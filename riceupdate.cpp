#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <map>
#include <ranges>
using namespace std;

using std::cout;
using std::system;

// Thanks to https://stackoverflow.com/a/44973498/524503 for these trim functions!

// trim from start (in place)
static inline void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));
}

// trim from end (in place)
static inline void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
        }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string& s) {
    rtrim(s);
    ltrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}

string getHomeDirectory() {
    system("echo $HOME > homeDirectory");

    ifstream fin("homeDirectory");

    string result;

    fin >> result;

    fin.close();

    return result;
}
string homeDirectory = getHomeDirectory();

struct Theme {
    string name;
    string accent;
    string bgSuffix = "unset";
    string bgDirectory = "unset";
    bool isDark = false;
    string bg;
    void setbg(string bg) { this->bg = bg; };
    string setBgCommand = "unset";
};

void fileReplaceAt(vector<string> toReplace, vector<string> replacement)
{
    ifstream filein("" + homeDirectory + "/.config/ricemood/ricemood.temp.ini"); // File to read from
    ofstream fileout("" + homeDirectory + "/.config/ricemood/ricemood.ini");     // Temporary file

    string strTemp;
    while (getline(filein, strTemp))
    {
        for (int i = 0; i < toReplace.size();i++) {

            string substring = strTemp.substr(0, toReplace[i].length());

            if (substring == toReplace[i])
            {
                strTemp = replacement[i];
            }
        }
        strTemp += "\n";
        fileout << strTemp;
    }
}

void classifyThemeFile(vector<Theme>& themes) {
    ifstream filein(homeDirectory + "/riceupdate/themes.conf"); // File to read from
    string curLine;
    Theme* curTheme;
    while (getline(filein, curLine)) {
        trim(curLine);
        for (int x = 0; x < curLine.size(); ++x) {
            if (curLine[x] == ';') {
                curLine = curLine.substr(0, x);
                break;
            }
        }

        if (curLine.substr(0, 10) == "[template]") {
            do {
                getline(filein, curLine);
            } while (curLine != "" && curLine[0] != '[');
        }

        if (curLine[0] == '[') {
            themes.push_back(Theme());
            curTheme = &themes[themes.size() - 1];
            curTheme->name = (curLine.substr(1));
            for (int x = 0; x < curTheme->name.size(); ++x) {
                if (curTheme->name[x] == ']') {
                    curTheme->name = curTheme->name.substr(0, x);
                    break;
                }
            }
        }

        if (curLine.substr(0, 6) == "accent")
            curTheme->accent = trim_copy((trim_copy(curLine.substr(6))).substr(1));

        if (curLine.substr(0, 8) == "bgSuffix")
            curTheme->bgSuffix = trim_copy((trim_copy(curLine.substr(8))).substr(1));

        if (ltrim_copy(curLine).substr(0, 11) == "bgDirectory") {
            curTheme->bgDirectory = trim_copy((trim_copy(curLine.substr(11))).substr(1));

            if (curTheme->bgDirectory[0] == '~') {
                curTheme->bgDirectory = homeDirectory + curTheme->bgDirectory.substr(1);
            }
        }


        if (ltrim_copy(curLine).substr(0, 6) == "isDark")
            curTheme->isDark = true; // default case is false

        if (ltrim_copy(curLine).substr(0, 12) == "setBgCommand")
            curTheme->setBgCommand = trim_copy((trim_copy(curLine.substr(11))).substr(1));
    }
    for (Theme& theme : themes) {
        theme.bg = theme.isDark ? "#111111" : "#565656";

        if (theme.setBgCommand == "unset") {
            if (theme.bgSuffix == "unset" && theme.bgDirectory != "unset")
                theme.setBgCommand = "feh --no-fehbg --zoom max --conversion-timeout 5 --bg-center '" + theme.bgDirectory + "' --image-bg '" + theme.bg + "'";

            else
                theme.setBgCommand = "feh --no-fehbg --zoom max --conversion-timeout 5 --bg-center '" + homeDirectory + "/riceupdate/background_svg/bg_colored.svg' --image-bg '" + theme.bg + "'";

        }

        theme.bgSuffix = (theme.bgSuffix == "unset" ? "fw" : theme.bgSuffix);
    }
}

int hex2dec(string s)
{
    unsigned int i;
    istringstream iss(s);
    iss >> std::hex >> i;

    return i;
}

int main(int argc, char* argv[])
{
    Theme selectedTheme;
    string argv1Placeholder = "";
    vector<Theme> themes;
    classifyThemeFile(themes);

    if (argc > 1)
    {
        if (string(argv[1]) == "-r")
        {
            ifstream cur_color_in(homeDirectory + "/riceupdate/cur_color");
            string cur_color;
            while (getline(cur_color_in, cur_color))
            {
                argv1Placeholder = cur_color;
            }
            cout << "Previous color: " << argv1Placeholder << endl;
            cur_color_in.close();
        }
        else
        {
            ofstream cur_color_rewrite(homeDirectory + "/riceupdate/cur_color");
            cur_color_rewrite << string(argv[1]);
            cur_color_rewrite.close();
        }
    }
    if (argc == 1)
    {
        for (Theme theme : themes) {
            cout << theme.name << endl;
        }
        return 0;
    }
    if (argc > 1) {
        if (string(argv[1]) == "--help") {
            for (Theme theme : themes) {
                cout << theme.name << endl;
            }
            return 0;
        }
        else {
            bool isColorSet = false;
            for (Theme theme : themes) {
                if (string(argv[1]) == theme.name || argv1Placeholder == theme.name) {
                    selectedTheme = theme;
                    isColorSet = true;
                    break;
                }
                else if (char(argv[1][0]) == '#' || argv1Placeholder[0] == '#')
                {
                    selectedTheme.accent = argv[1];
                    isColorSet = true;
                    if (argc < 1) {
                        if (char(argv[2][0]) == '#') {
                            selectedTheme.bg = argv[2];
                        }
                    }
                    selectedTheme.setBgCommand = "feh --no-fehbg --conversion-timeout 5 --bg-center '" + homeDirectory + "/riceupdate/background_svg/bg_colored.svg' --image-bg '" + selectedTheme.bg + "'";
                    break;
                }
            }
            if (!isColorSet) {
                cout << argv[1][0] << endl;
                cout << "enter a valid color" << endl;
                return 0;
            }
        }
    }

    string backgroundChangeCommand = "sed -e 's/#000000/" + selectedTheme.accent + "/' " + homeDirectory + "/riceupdate/background_svg/bg_template_" + selectedTheme.bgSuffix + ".svg > " + homeDirectory + "/riceupdate/background_svg/bg_colored.svg";

    string loginChangeFGCommand = "sudo sed -e 's/#00ee00/" + selectedTheme.accent + "/' /usr/share/sddm/themes/sddm-sugar-dark/background_template.svg > /usr/share/sddm/themes/sddm-sugar-dark/background_template1.svg";
    string loginChangeBGCommand = "sudo sed -e 's/#0000ee/" + selectedTheme.bg + "/' /usr/share/sddm/themes/sddm-sugar-dark/background_template1.svg > /usr/share/sddm/themes/sddm-sugar-dark/background.svg";

    system(backgroundChangeCommand.c_str());
    system(loginChangeFGCommand.c_str());
    system(loginChangeBGCommand.c_str());

    int accRed = hex2dec(selectedTheme.accent.substr(1, 2));
    int accBlue = hex2dec(selectedTheme.accent.substr(3, 2));
    int accGreen = hex2dec(selectedTheme.accent.substr(5, 2));
    int bgRed = hex2dec(selectedTheme.bg.substr(1, 2));
    int bgBlue = hex2dec(selectedTheme.bg.substr(3, 2));
    int bgGreen = hex2dec(selectedTheme.bg.substr(5, 2));
    int rgbAvg = (accRed + accBlue + accGreen) / 3;
    int bgAvg = (bgRed + bgBlue + bgGreen) / 3;
    int textCol = 255;
    int bgTextCol = 0;

    if (accRed <= 0)
    {
        accRed = 1;
    }
    if (accBlue <= 0)
    {
        accBlue = 1;
    }
    if (accGreen <= 0)
    {
        accGreen = 1;
    }

    int darknessThreshold = 170;
    int pbDarknessThreshold = 80;

    if (rgbAvg < darknessThreshold)
    {
        textCol = 255;
    }
    else
    {
        textCol = 1;
    }

    if (bgAvg < pbDarknessThreshold)
    {
        bgTextCol = 255;
    }
    else
    {
        bgTextCol = 1;
    }

    string accentFormatted = "accent1 = '@V>r(" + to_string(accRed) + ")>g(" + to_string(accBlue) + ")>b(" + to_string(accGreen) + ")'";
    string textColor = "textCol = '@V>r(" + to_string(textCol) + ")>g(" + to_string(textCol) + ")>b(" + to_string(textCol) + ")'";
    string bgColor = "backgroundCol = '@V>r(" + to_string(bgRed) + ")>g(" + to_string(bgBlue) + ")>b(" + to_string(bgGreen) + ")'";
    string bgText = "pbTextCol = '@V>r(" + to_string(bgTextCol) + ")>g(" + to_string(bgTextCol) + ")>b(" + to_string(bgTextCol) + ")'";
    vector<string> textReplacements = { accentFormatted, textColor, bgColor, bgText };
    fileReplaceAt(vector<string>{"accent1", "textCol", "backgroundCol", "pbTextCol"}, textReplacements);
    system("echo Y | ricemood -a > recent_log");
    system("i3-msg restart > recent_log");
    sleep(1.5);
    system("i3-msg restart > recent_log");

    system("killall dunst > recent_log");
    string dunstmsg = "notify-send 'Theme Changed!' 'Current Color: " + string(argv[1]) + "'";
    system(dunstmsg.c_str());

    if (argc > 3) {
        cout << argv[2] << endl;
        if (argv[2] == (string)"--bg") {
            selectedTheme.setBgCommand = "feh --no-fehbg --conversion-timeout 5 --bg-center " + (string)"'" + argv[3] + "'" + " --image-bg '#111111'";
        }
    }

    cout << selectedTheme.setBgCommand << endl;
    system(selectedTheme.setBgCommand.c_str());

    cout << "Replacing PATH link" << endl;
    system("sudo rm /usr/bin/riceupdate");
    system(("sudo ln " + homeDirectory + "/riceupdate/riceupdate /usr/bin/riceupdate").c_str());

    cout << "Running Gradience" << endl;
    system((string("gradience-cli monet -p ~/background.svg -n Monet --theme ") + (selectedTheme.isDark ? "dark" : "light")).c_str());
    system("gradience-cli apply -n Monet --gtk both");

    return 0;
}

