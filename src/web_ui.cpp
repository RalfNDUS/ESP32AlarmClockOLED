#include <WiFi.h>
#include <time.h>
#include "global.h"
#include "alarm.h"

String htmlHeader(const String& title) {
  String html =
    "<!DOCTYPE html><html lang=\"de\"><head>"
    "<meta charset=\"utf-8\" />"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />"
    "<meta name=\"theme-color\" content=\"#121212\" />"
    "<title>" + title + "</title>"
    "<style>"
      /* ===== Farbvariablen: dunkles Grauschema ===== */
      ":root{"
        "--bg:#0b0b0b;"
        "--panel:#181818;"
        "--panel-border:#2a2a2a;"
        "--text:#e0e0e0;"
        "--muted:#9e9e9e;"
        "--accent:#3a3a3a;"
        "--accent-hover:#505050;"
        "--secondary:#2a2a2a;"
        "--secondary-text:#e0e0e0;"
        "--status:#111111;"
        "--input-bg:#1a1a1a;"
        "--input-border:#333333;"
        "--shadow:0 8px 24px rgba(0,0,0,.45);"
        "--radius:14px;"
        "--gap:12px;"
      "}"
      "html,body{background:var(--bg);color:var(--text);}"
      "body{font-family:system-ui,-apple-system,Segoe UI,Roboto,Ubuntu,\"Helvetica Neue\",Arial,sans-serif;"
            "margin:20px;line-height:1.45;-webkit-font-smoothing:antialiased;}"

      ".card{max-width:780px;margin:auto;background:var(--panel);"
            "border:1px solid var(--panel-border);border-radius:var(--radius);"
            "padding:18px;box-shadow:var(--shadow)}"

      "h1{margin:0 0 12px 0;font-size:1.35rem;font-weight:700}"

      ".grid{display:grid;grid-template-columns:1fr 1fr;gap:var(--gap)}"
      ".kv{display:grid;grid-template-columns:160px 1fr;gap:var(--gap)}"

      "label{display:block;font-weight:600;margin-bottom:6px;color:var(--text)}"

      "input[type=\"number\"],input[type=\"text\"],input[type=\"time\"],select{"
        "width:100%;padding:10px 12px;border:1px solid var(--input-border);"
        "border-radius:10px;background:var(--input-bg);color:var(--text);"
        "outline:none;transition:border-color .2s ease;"
      "}"
      "input:focus{border-color:#777;box-shadow:0 0 0 3px rgba(255,255,255,.1);}"
      "input[type=\"checkbox\"]{transform:scale(1.25);margin-right:8px;vertical-align:middle;accent-color:#888;}"

      /* ===== Einheitliche Buttons ===== */
      "button,.btn{"
        "display:inline-block;"
        "font-size:1rem;"
        "line-height:1.2;"
        "background:var(--accent);"
        "color:#f0f0f0;"
        "border:none;"
        "border-radius:10px;"
        "padding:10px 14px;"
        "font-weight:700;"
        "cursor:pointer;"
        "text-align:center;"
        "transition:background .2s ease, transform .1s ease;"
      "}"
      "a.btn, a.btn:hover, a.btn:active, a.btn:visited {text-decoration: none !important; color: inherit;}"
      "button:hover,.btn:hover{background:var(--accent-hover);transform:translateY(-1px);}"
      ".btn-secondary{background:var(--secondary);color:var(--secondary-text)}"
      ".btn-secondary:hover{filter:brightness(1.2)}"

      ".status{padding:12px;border-radius:12px;background:var(--status);"
        "border:1px solid var(--panel-border);margin:12px 0}"

      ".inline{display:flex;gap:8px;align-items:center}"

      "footer{margin-top:14px;color:var(--muted);font-size:.9rem;text-align:center}"

      "#time,#date{display:inline} #time{margin-right:6px}"
    "</style></head><body><div class=\"card\">";
  return html;
}

String javaScript() {
  String js;
  js += "<script>";
  js += "function updateData(){";
  js += "fetch('/data').then(r => r.json()).then(d => {";
  js += "document.getElementById('time').textContent = d.date + ' · ' + d.time;";
  js += "document.getElementById('outdoorTemp').textContent = d.outdoorTemp.toFixed(1) + ' °C';";
  js += "});";
  js += "}";
  js += "setInterval(updateData, 10000);";
  js += "window.onload = updateData;";
  js += "</script>";
  return js;
}

String htmlFooter() {
  return R"====(</div></body></html>)====";
}

String htmlIndex() {
  auto& s = appState();

  String currentTime = s.currentTimeString;
  String currentDate = s.currentDateString;
  String h;

  h += htmlHeader("ESP32 AlarmClock");
  h += "<h1>ESP32 AlarmClock</h1>";
  h += "<div class='status'><div class='kv'>";
  h += "<div>Zeit:</div><div><strong id='time'>" + currentDate + " · " + currentTime + "</strong></div>";
  h += "<div>Au&#223;entemperatur:</div><div><strong id='outdoorTemp'>";
  h += (!isnan(s.outdoorTemp)) ? String(s.outdoorTemp,1) + " &#176;C" : String("--.- &#176;C");
  h += "</strong></div>";
  h += "<div>WLAN-IP:</div><div>" + WiFi.localIP().toString() + "</div>";
  h += "<div>mDNS:</div><div>" + String(MDNS_NAME) + ".local</div>";
  h += "<div>Alarmstatus:</div><div>" + String(s.alarmTriggered ? "<strong>AKTIV</strong>" : "inaktiv") + "</div>";
  h += String("<div>Alarm 1:</div><div>") + s.alarm1TimeStr + "</div>";
  h += String("<div>Alarm 2:</div><div>") + s.alarm2TimeStr + "</div>";
  h += "</div></div>";
  h += "<form class='grid' action='/set' method='GET'>";
  h += "<div><label>Alarm 1 (hh:mm)</label>";
  int alarm1hour = s.alarm1Time / 100;
  int alarm1min  = s.alarm1Time % 100;
  int alarm2hour = s.alarm2Time / 100;
  int alarm2min  = s.alarm2Time % 100;
  h += "<div class='inline'><input type='number' name='a1h' min='0' max='23' value='" + String(alarm1hour) + "'>";
  h += "<span>:</span><input type='number' name='a1m' min='0' max='59' value='" + String(alarm1min) + "'></div>";
  h += "<div class='inline' style='margin-top:8px'><input type='checkbox' name='a1e' ";
  if (s.alarm1Enabled) h += "checked";
  h += "><span>aktiv</span></div></div>";
  h += "<div><label>Alarm 2 (hh:mm)</label>";
  h += "<div class='inline'><input type='number' name='a2h' min='0' max='23' value='" + String(alarm2hour) + "'>";
  h += "<span>:</span><input type='number' name='a2m' min='0' max='59' value='" + String(alarm2min) + "'></div>";
  h += "<div class='inline' style='margin-top:8px'><input type='checkbox' name='a2e' ";
  if (s.alarm2Enabled) h += "checked";
  h += "><span>aktiv</span></div></div>";
  h += "<div class='inline' style='grid-column:1/-1;margin-top:8px'>";
  h += "<button type='submit'>Speichern</button>";
  h += "<a class='btn btn-secondary' href='/test'>Test-Alarm</a>";
  h += "<a class='btn btn-secondary' href='/stop'>Stopp</a>";
  h += "</div></form>";
  h += javaScript();
  h += htmlFooter();
  return h;
}
