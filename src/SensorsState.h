
#define PropWithSetter(type, name) \
    type name; \
    void Set##name(type value) { \
        if (value == name) { return; }\
        name = value; \
        _stateChanged = true; \
    }

struct SensorsState {

    static const int SensorsCount = 9;
    
    int Uptime;
    PropWithSetter(float, InsideTemperatureQuinping)
    PropWithSetter(float, InsideTemperatureDs)
    PropWithSetter(float, OutsideTemperature)
    PropWithSetter(int, Humidity)
    PropWithSetter(int, Battery)
    PropWithSetter(int, Pm25)
    PropWithSetter(int, Pm10)
    PropWithSetter(int, CarbonDioxide)

    String UptimeString = String("No data");

    void SetUptime(int uptime) {
        Uptime = uptime;
        UptimeString = TimespanToString(uptime);
    }

    bool IsStateChanged() {
        if (!_stateChanged) { return false; }
        _stateChanged = false;
        return true;
    }

private:

    bool _stateChanged = false;

    static String TimespanToString(int totalSeconds) {
        int days = totalSeconds / (24 * 3600);
        totalSeconds %= (24 * 3600);
        int hours = totalSeconds / 3600;
        totalSeconds %= 3600;
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;
        String result;
        if (days > 0) {
            result += days;
            if (days > 1) {
                result += " days ";
            } else {
                result += " day ";
            }
        }
        result += hours;
        result += ":";
        result += minutes;
        result += ":";
        result += seconds;
        return result;
    }

};

