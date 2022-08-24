#include <BluetoothSerial.h>
#include <DS3231.h>
#include <Wire.h>
#include <string>

namespace detail {
#define ADD_ACT(func)                                                          \
  { #func, func }

namespace STATUS {
const bool DISABLE = 0;
const bool ENABLE = 1;
}; // namespace STATUS

namespace DEFAULTS {
const uint16_t BAUD_RATE = 9600;
const char *BT_NAME = "scc-farm";
const uint8_t IRRIGATION_NUM = 2;
const uint32_t IRR_DURATION = 360; // seconds
};                                 // namespace DEFAULTS

namespace PIN {
const uint8_t RELAY = 2;
const uint8_t SCL = 22;
const uint8_t SDA = 21;
}; // namespace PIN

const String EMPTY_STRING = emptyString; // Alias

struct string_view {
  const char *begin;
  const char *end;
  size_t len;

  string_view(const char *begin, const char *end)
      : begin{begin}, end{end}, len{size_t(end - begin)} {}
  string_view(const char *string_literal)
      : begin{string_literal}, end{strchr(string_literal, '\0')}, len{size_t(
                                                                      end -
                                                                      begin)} {}
  string_view() = delete;

  bool operator==(const string_view &);
  uint16_t get_len() const { return this->len; }
};

void show_time();
String bt_readline();
byte get_hour();
byte get_minute();
void do_irr();

void action(const String &cmd);
void relay(bool enable);
void set_time(const string_view &s); // set irrigation time
void active_relay(const string_view &s);

BluetoothSerial BT;
RTClib rtc__;
uint32_t irr_duration = DEFAULTS::IRR_DURATION;
bool irred = false;

using Action = void (*)(const string_view &);
struct action_table__ {
  const char *name;
  Action act;
} action_table[10] = {
    ADD_ACT(set_time),
    ADD_ACT(active_relay),
    {nullptr, nullptr},
};

struct irr_time__ {
  byte hour;
  byte minute;
} irr_time[DEFAULTS::IRRIGATION_NUM] = {
    {.hour = 18, .minute = 0},
    {.hour = 6, .minute = 0},
};
}; // namespace detail

void setup() {
  using namespace detail;
  Wire.begin(); // Enable I2C
  Serial.begin(DEFAULTS::BAUD_RATE);
  BT.begin(DEFAULTS::BAUD_RATE);
  BT.begin(DEFAULTS::BT_NAME);

  // Init RTC time if it is essential
  auto set_rtc = []() {
    DS3231 rtc;
    rtc.setHour(8);
    rtc.setMinute(45);
  };
}

void loop() {
  using namespace detail;
  const String cmd = bt_readline();
  if (cmd != EMPTY_STRING)
    action(cmd);

  // if time is due, do irrigate
  do_irr();
}

void detail::relay(bool enable) {
  using namespace detail;
  digitalWrite(PIN::RELAY, enable);
}

void detail::show_time() {
  const auto now = rtc__.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}

String detail::bt_readline() {
  if (BT.available())
    return BT.readStringUntil('\n');
  else
    return EMPTY_STRING;
}

// Format:
// <cmd> = (<key>:<value>' ')*\n
// <key>,<value> are non-whitespace strings, both of them could not have ':'
// char each <key>,<value> pair has a whitespace at the end
void detail::action(const String &cmd) {
  const char *cur = cmd.begin();
  while (*cur != '\n') {
    string_view key = string_view(cur, strchr(cur, ':'));
    int index = 0;
    while (action_table[index].name != nullptr) {
      if (string_view(action_table[index].name) == key) {
        const char *vb = cur + 1;
        string_view value = string_view(vb, strchr(vb, ' '));
        action_table[index].act(value);
        // Move cur to the next begin of <key>,<value> pair
        cur = vb + value.get_len() + 1;
      }
      ++index;
    }
  }
}

bool detail::string_view::operator==(const string_view &rhs) {
  if (this->len != rhs.len)
    return false;

  for (size_t i = 0; i < this->len; i++)
    if (this->begin[i] != rhs.begin[i])
      return false;

  return true;
}

// s is 'hh.mm.index'
// index holds one byte
void detail::set_time(const string_view &s) {
#define B2I(b) ((b) - '0')

  byte hour = B2I(s.begin[0]) * 10 + B2I(s.begin[1]);
  byte minute = B2I(s.begin[3]) * 10 + B2I(s.begin[4]);
  byte index = B2I(s.begin[6]);

  irr_time[index].hour = hour;
  irr_time[index].minute = minute;
}

// s is "1" or "0"
// "1" is enable
// "0" is disable
void detail::active_relay(const string_view &s) { relay(*s.begin ^ '0'); }

byte detail::get_hour() {
  const auto &now = rtc__.now();
  return now.hour();
}

byte detail::get_minute() {
  const auto &now = rtc__.now();
  return now.minute();
}

void detail::do_irr() {
  // Not support preemption now
  irr_time__ cur_time;
  cur_time.hour = get_hour();
  cur_time.minute = get_minute();
  auto time_conv = [](irr_time__ t) -> uint32_t {
    return ((uint32_t)t.hour) * 60UL + t.minute;
  };

  static constexpr size_t irr_t_len = sizeof(irr_time) / sizeof(*irr_time);
  for (size_t i = 0; i < irr_t_len; i++) {
    if (time_conv(cur_time) > time_conv(irr_time[i]) &&
        time_conv(cur_time) < time_conv(irr_time[i]) + irr_duration)
      relay(STATUS::ENABLE);
    else
      relay(STATUS::DISABLE);
  }
}