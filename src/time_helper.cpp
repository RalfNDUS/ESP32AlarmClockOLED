#include <Arduino.h>
#include <time.h>

static const double LAT = 51.227;   // Düsseldorf
static const double LON =  6.773;   // Ost positiv

// Hilfsfunktionen
static double deg2rad(double d) { return d * M_PI / 180.0; }
static double rad2deg(double r) { return r * 180.0 / M_PI; }
static double wrap360(double x){ double r=fmod(x,360.0); return (r<0)? r+360.0 : r; }
static double wrap24(double h){ double r=fmod(h,24.0);   return (r<0)? r+24.0 : r; }

// Tagnummer im Jahr (1..366)
static int dayOfYear(int y,int m,int d){
  static const int cum[12] = {0,31,59,90,120,151,181,212,243,273,304,334};
  bool leap = ( (y%4==0 && y%100!=0) || (y%400==0) );
  int n = cum[m-1] + d;
  if(leap && m>2) n += 1;
  return n;
}

// NOAA-Formel: liefert lokale Stunden (0..24) für Sunrise/Sunset
// tzOff = Stunden offset zur UTC (CET=+1, CEST=+2)
static void solarRiseSetLocal(int year,int month,int day, double lat, double lon, double tzOff,
                              double* sunrise_h, double* sunset_h, bool* always_day, bool* always_night)
{
  int N = dayOfYear(year,month,day);
  const double zenith = 90.833;    // -0.833° für Refraktion/oberen Rand
  const double D2R = M_PI/180.0;

  auto calcTime = [&](bool sunrise)->double {
    double lngHour = lon / 15.0;
    double t = N + ((sunrise ? 6.0 : 18.0) - lngHour) / 24.0;            // ungefähre Zeit
    double M = 0.9856 * t - 3.289;                                       // mittlere Anomalie
    double L = M + 1.916*sin(D2R*M) + 0.020*sin(D2R*2*M) + 282.634;      // ekliptische Länge
    L = wrap360(L);

    double RA = rad2deg(atan(0.91764 * tan(D2R*L)));
    RA = wrap360(RA);

    // Quadrantenanpassung
    double Lquadrant  = floor(L/90.0) * 90.0;
    double RAquadrant = floor(RA/90.0) * 90.0;
    RA = RA + (Lquadrant - RAquadrant);
    RA /= 15.0; // in Stunden

    double sinDec = 0.39782 * sin(D2R*L);
    double cosDec = cos(asin(sinDec));

    double cosH = (cos(D2R*zenith) - (sinDec * sin(D2R*lat))) / (cosDec * cos(D2R*lat));

    // Polar-Grenzfälle
    if (cosH >  1.0) return NAN; // Sonne geht nie auf
    if (cosH < -1.0) return INFINITY; // Sonne geht nie unter

    double H = sunrise ? 360.0 - rad2deg(acos(cosH)) : rad2deg(acos(cosH));
    H /= 15.0; // Stunden

    double T = H + RA - (0.06571 * t) - 6.622;           // lokale mittlere Zeit
    double UT = T - lngHour;                             // in UTC
    UT = wrap24(UT);
    double local = UT + tzOff;                           // lokale Zeit
    local = wrap24(local);
    return local;
  };

  double r = calcTime(true);
  double s = calcTime(false);

  *always_night = isnan(r);       // nie Aufgang -> polare Nacht
  *always_day   = isinf(s);       // nie Untergang -> polare Sonne
  *sunrise_h = isnan(r)? NAN : r;
  *sunset_h  = isinf(s)? NAN : s;
}

// Minuten (0..1439) aus Stundenbruch
static int hmToMinutes(double h) {
  if (isnan(h)) return -1;
  int hh = (int)floor(h);
  int mm = (int)floor((h - hh) * 60.0 + 0.5);
  return hh*60 + (mm%60);
}

bool isDaytime(const struct tm &lt)
{
  // Zeitzonen-Offset (CET=1, CEST=2)
  int tzOffsetHours = 1 + (lt.tm_isdst > 0 ? 1 : 0);

  double sunriseH = 0, sunsetH = 0;
  bool alwaysDay = false, alwaysNight = false;

  // Düsseldorf-Koordinaten
  const double LAT = 51.227;
  const double LON = 6.773;

  solarRiseSetLocal(1900 + lt.tm_year, lt.tm_mon + 1, lt.tm_mday,
                    LAT, LON, tzOffsetHours, &sunriseH, &sunsetH,
                    &alwaysDay, &alwaysNight);

  int sunriseMin = hmToMinutes(sunriseH);
  int sunsetMin  = hmToMinutes(sunsetH);
  int nowMin     = lt.tm_hour * 60 + lt.tm_min;

  bool result;
  if (alwaysDay) { result = true; }         // Sonne geht nie unter 
    else if (alwaysNight) { result = false; }  // Sonne geht nie auf
   else {
    // Nacht = nach Sonnenuntergang oder vor Sonnenaufgang
    result = (nowMin >= sunsetMin || nowMin < sunriseMin) ? false : true;
  }

  return result;
}
