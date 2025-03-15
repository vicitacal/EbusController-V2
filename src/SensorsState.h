
struct SensorsState {

    static const int SensorsCount = 9;
    
    int Uptime;
    float InsideTemperatureQuinping;
    float InsideTemperatureDs;
    float OutsideTemperature;
    int Humidity;
    int Battery;
    int Pm25;
    int Pm10;
    int CarbonDioxide;

    String UptimeString = String("No data");

    void SetUptime(int uptime) {
        Uptime = uptime;
        UptimeString = TimespanToString(uptime);
    }

private:

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

