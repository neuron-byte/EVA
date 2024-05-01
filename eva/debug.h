#pragma once

// #define DEBUG
// #define WARNING
// #define INFO
// #define ERROR

#ifdef DEBUG
#define debugf(in, format) Serial.printf(("[debug] " in), format)
#define debugln(in) Serial.println(("[debug] " in))
#else
#define debugf(in, format)
#define debugln(in)
#endif


#ifdef WARNING
#define warningln(in) Serial.println(("[warning] " in))
#define warningf(in, format) Serial.printf(("[warning] " in), format)
#else
#define warningln(in)
#define warningf(in, format)
#endif

#ifdef WARNING
#define infoln(in) Serial.println(("[info] " in))
#define infof(in, format) Serial.printf(("[info] " in), format)
#define info(in) Serial.print(("[info] " in))
#else
#define infoln(in)
#define infof(in, format)
#endif

#ifdef ERROR
#define errorln(in) Serial.println(("[error] " in))
#define errorf(in, format) Serial.printf(("[error] " in), format)
#else
#define errorln(in)
#define errorf(in, format)
#endif
