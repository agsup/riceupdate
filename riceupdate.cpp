#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <vector>
#include <map>
using namespace std;

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
    Theme(string name, string accent, bool isDark = false, string bgSuffix = "fw", string setBg = "unset") {
        this->name = name;
        this->accent = accent;
        this->bgSuffix = bgSuffix;
        this->isDark = isDark;
        this->bg = isDark ? "#111111" : "#565656";
        this->setBg = setBg;
        if (this->setBg == "unset") {
            this->setBg = "feh --no-fehbg --conversion-timeout 5 --bg-center '" + homeDirectory + "/riceupdate/background_svg/bg_colored.svg' --image-bg '" + bg + "'";
        }
    }

    Theme() {}; // Todo: Define defaults

    string name;
    string accent;
    string bgSuffix;
    string setBg;
    string bg;
    bool isDark;
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
            // cout << substring << endl;
            if (substring == toReplace[i])
            {
                strTemp = replacement[i];
            }
        }
        strTemp += "\n";
        fileout << strTemp;
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

    vector<Theme> themes = {
        {"abby","#007678"},
        {"abby-bg","#007678",false,"fw","feh --no-fehbg --zoom max --conversion-timeout 5 --bg-center '" + homeDirectory + "/Downloads/hydrangeas.jpg' --image-bg '#111111'"},
        {"bulb", "#fffd8a", true, "bulb"},
        {"bowsette","#FFBD94", true},
        {"delta", "#FF3333",true, "delta"},
        {"glowstick", "#05eb7f", true},
        {"grey", "#525252"},
        {"hot-pink","#FF3377"} ,
        {"lavender", "#bf91ff"} ,
        {"lavender-bg", "#bf91ff",false,"fw","feh --no-fehbg --zoom max --conversion-timeout 5 --bg-center '" + homeDirectory + "/Downloads/lavender.jpg' --image-bg '#111111'"},
        {"mint", "#2fd688"},
        {"mocha","#875c3c"},
        {"monkey-dark","#bf6414", true},
        {"monkey-light","#914e31"},
        {"peach", "#FFBD94"},
        {"periwinkle", "#CCCCFF"},
        {"perilwinkle", "#CCCCFF", true},
        {"peppermint","#FF6666"},
        {"sand","#FFDD99"},
        {"spring-green", "#98FB98"},
        {"sakura", "#F7C3F5",false,"fw","feh --no-fehbg --conversion-timeout 5 --bg-center '" + homeDirectory + "/Downloads/cherryblossom.jpg' --image-bg '#111111'"},
        {"sky","#5c9aff"},
        {"techniviolet", "#8d5cff",true}
    };

    if (argc > 1)
    {
        if (string(argv[1]) == "-r")
        {
            ifstream cur_color_in("" + homeDirectory + "/riceupdate/cur_color");
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
            ofstream cur_color_rewrite("" + homeDirectory + "/riceupdate/cur_color");
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
                    selectedTheme.setBg = "feh --no-fehbg --conversion-timeout 5 --bg-center '" + homeDirectory + "/riceupdate/background_svg/bg_colored.svg' --image-bg '" + selectedTheme.bg + "'";
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
    // cout << rgbAvg << endl;
    if (rgbAvg < darknessThreshold)
    {
        textCol = 255;
    }
    else
    {
        textCol = 1;
    }

    // cout << bgAvg << endl;
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
    // cout << "done" << endl;
    if (argc > 3) {
        cout << argv[2] << endl;
        if (argv[2] == (string)"--bg") {
            selectedTheme.setBg = "feh --no-fehbg --conversion-timeout 5 --bg-center " + (string)"'" + argv[3] + "'" + " --image-bg '#111111'";
        }
    }

    cout << selectedTheme.setBg << endl;
    system(selectedTheme.setBg.c_str());

    cout << "Replacing PATH link" << endl;
    system("sudo rm /usr/bin/riceupdate");
    system(("sudo ln " + homeDirectory + "/riceupdate/riceupdate /usr/bin/riceupdate").c_str());

    cout << "Running Gradience" << endl;
    system((string("gradience-cli monet -p ~/background.svg -n Monet --theme ") + (selectedTheme.isDark ? "dark" : "light")).c_str());
    system("gradience-cli apply -n Monet --gtk both");

    return 0;
}

