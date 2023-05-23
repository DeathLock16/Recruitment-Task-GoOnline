#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <conio.h>

using std::vector;
using std::string;
using std::stringstream;
using std::stoi;
using std::exception;
using std::transform;
using std::isxdigit;
using std::max;
using std::min;
using std::ifstream;
using std::getline;
using std::cout;
using std::endl;
using std::hex;

// Struct to represent a color
struct Color {
    int red,
        green,
        blue,
        alpha;
};

// Utility function to split a string by a delimiter
vector<string> splitString(const string& _s, char _delimiter) {
    vector<string> tokens;
    stringstream ss(_s);
    string token;

    while (getline(ss, token, _delimiter))
        if (!token.empty())
            tokens.push_back(token);

    return tokens;
}

// Utility function to parse a color from a string
bool parseColor(const string& _colorStr, Color& _color) {
    vector<string> components = splitString(_colorStr, ',');
    if (components.size() != 4)
        return false;

    try {
        _color.red = stoi(components[0]);
        _color.green = stoi(components[1]);
        _color.blue = stoi(components[2]);
        _color.alpha = stoi(components[3]);
    } catch (const exception&) { return false; }
    
    return true;
}

// Utility function to validate a hex color
bool isValidHexColor(const string& _hexColor) { // Hex color regex: /^([0-9a-f]{3}|[0-9a-f]{6}|[0-9a-f]{8})$/i
    string hexColorCopy = _hexColor;
    transform(hexColorCopy.begin(), hexColorCopy.end(), hexColorCopy.begin(), ::tolower);

    if (hexColorCopy.size() != 3 && hexColorCopy.size() != 6 && hexColorCopy.size() != 8)
        return false;

    for (char c : hexColorCopy)
        if (!isxdigit(c))
            return false;

    return true;
}

// Utility function to parse a hex color
bool parseHexColor(const string& _hexColor, Color& _color) {
    if (!isValidHexColor(_hexColor))
        return false;

    string hexColorCopy = _hexColor;
    transform(hexColorCopy.begin(), hexColorCopy.end(), hexColorCopy.begin(), ::tolower);

    if (hexColorCopy.size() == 3) { // Expand shorthand hex color
        string expandedHexColor;
        for (char c : hexColorCopy)
            expandedHexColor += string(2, c);
        hexColorCopy = expandedHexColor;
    }
    
    try {
        _color.red = stoi(hexColorCopy.substr(0, 2), nullptr, 16);
        _color.green = stoi(hexColorCopy.substr(2, 2), nullptr, 16);
        _color.blue = stoi(hexColorCopy.substr(4, 2), nullptr, 16);

        if (hexColorCopy.size() == 8)
            _color.alpha = stoi(hexColorCopy.substr(6, 2), nullptr, 16);
        else _color.alpha = 255;

    } catch (const exception&) { return false; }
    
    return true;
}

// Utility function to calculate hue from RGB components
double calculateHue(int _red, int _green, int _blue) {
    double r = _red / 255.0,
            g = _green / 255.0,
            b = _blue / 255.0;

    double maxVal = max({ r, g, b }),
            minVal = min({ r, g, b });

    double hue = 0.0;

    if (maxVal == minVal)
        hue = 0.0;
    else if (maxVal == r)
        hue = 0.0 + ((g - b) / (maxVal - minVal));
    else if (maxVal == g)
        hue = 2.0 + ((b - r) / (maxVal - minVal));
    else if (maxVal == b)
        hue = 4.0 + ((r - g) / (maxVal - minVal));
    hue *= 60.0;

    if (hue < 0.0)
        hue += 360.0;

    return hue;
}

// Utility function to calculate saturation from RGB components
double calculateSaturation(int _red, int _green, int _blue) {
    double r = _red / 255.0,
            g = _green / 255.0,
            b = _blue / 255.0;

    double maxVal = max({ r, g, b }),
            minVal = min({ r, g, b });

    double saturation = 0.0;
    if (maxVal != 0.0)
        saturation = (maxVal - minVal) / maxVal;
    return saturation;
}

// Utility function to calculate lightness from RGB components
double calculateLightness(int _red, int _green, int _blue) {
    double r = _red / 255.0,
            g = _green / 255.0,
            b = _blue / 255.0;

    double maxVal = max({ r, g, b }),
            minVal = min({ r, g, b });

    return 0.5 * (maxVal + minVal);
}

// Utility function to print color information
void printColorInfo(const Color& _color) {
    cout << endl;
    cout << "Red: " << _color.red << endl;
    cout << "Green: " << _color.green << endl;
    cout << "Blue: " << _color.blue << endl;
    cout << "Alpha: " << _color.alpha << endl;
    cout << "Hex: #" << hex << (_color.red << 24 | _color.green << 16 | _color.blue << 8 | _color.alpha) << endl;
    cout << "Hue: " << calculateHue(_color.red, _color.green, _color.blue) << endl;
    cout << "Saturation: " << calculateSaturation(_color.red, _color.green, _color.blue) << endl;
    cout << "Lightness: " << calculateLightness(_color.red, _color.green, _color.blue) << endl;
}

int main(int argc, char* argv[]) {
    string mode = "mix"; // Default mode
    vector<Color> colors;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--mode" || arg == "-m")
            if (i + 1 < argc) {
                mode = argv[i + 1];
                i++; // Skip the next argument
            }
        else {
            Color color;
            if (parseHexColor(arg, color) || parseColor(arg, color))
                colors.push_back(color);
            else cout << "Invalid color: " << arg << endl;
        }
    }

    // Read colors from file
    ifstream file("colors.txt");    
    if (file.is_open()) {
        string line;

        while (getline(file, line)) {
            Color color;

            if (parseHexColor(line, color) || parseColor(line, color))
                colors.push_back(color);
            else {
                if (line.empty()) continue;
                cout << "Invalid color in file: " << line << endl;
            }
        }
        
        file.close();
    } else cout << "Failed to open file 'colors.txt'" << endl;

    // Perform color calculations based on mode
    if (colors.empty())
        cout << "No colors provided" << endl;
    else {
        cout << colors.size() << " colors has been loaded" << endl;

        if (mode == "lowest") {
            Color lowestColor = colors[0];

            for (size_t i = 1; i < colors.size(); i++) {
                lowestColor.red = min(lowestColor.red, colors[i].red);
                lowestColor.green = min(lowestColor.green, colors[i].green);
                lowestColor.blue = min(lowestColor.blue, colors[i].blue);
                lowestColor.alpha = min(lowestColor.alpha, colors[i].alpha);
            }
            
            printColorInfo(lowestColor);
        }
        
        else if (mode == "highest") {
            Color highestColor = colors[0];

            for (size_t i = 1; i < colors.size(); i++) {
                highestColor.red = max(highestColor.red, colors[i].red);
                highestColor.green = max(highestColor.green, colors[i].green);
                highestColor.blue = max(highestColor.blue, colors[i].blue);
                highestColor.alpha = max(highestColor.alpha, colors[i].alpha);
            }

            printColorInfo(highestColor);
        }
        
        else if (mode == "mix-saturate") {
            int totalRed = 0,
                totalGreen = 0,
                totalBlue = 0,
                totalAlpha = 0;

            for (const Color& _color : colors) {
                totalRed += _color.red;
                totalGreen += _color.green;
                totalBlue += _color.blue;
                totalAlpha += _color.alpha;
            }

            double averageSaturation = calculateSaturation(totalRed / colors.size(),
                                                            totalGreen / colors.size(),
                                                            totalBlue / colors.size());
            Color lastColor = colors.back();
            lastColor.red = totalRed / colors.size();
            lastColor.green = totalGreen / colors.size();
            lastColor.blue = totalBlue / colors.size();
            lastColor.alpha = totalAlpha / colors.size();
            printColorInfo(lastColor);
            cout << "Average Saturation: " << averageSaturation << endl;
        }
        
        else { // Default mode is "mix"
            int totalRed = 0,
                totalGreen = 0,
                totalBlue = 0,
                totalAlpha = 0;

            for (const Color& _color : colors) {
                totalRed += _color.red;
                totalGreen += _color.green;
                totalBlue += _color.blue;
                totalAlpha += _color.alpha;
            }
            
            Color mixedColor;
            mixedColor.red = totalRed / colors.size();
            mixedColor.green = totalGreen / colors.size();
            mixedColor.blue = totalBlue / colors.size();
            mixedColor.alpha = totalAlpha / colors.size();
            printColorInfo(mixedColor);
        }
    }

    _getch();

    return 0;
}