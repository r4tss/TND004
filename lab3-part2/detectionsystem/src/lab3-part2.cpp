#include <linesdiscoverysystem/readfiles.h>

#include <vector>
#include <string>
#include <filesystem>

#include <rendering/window.h>
#include <fmt/format.h>

#include <map>
#include <unordered_map>
#include <set>
#include <fstream>

void plotData(const std::string& name);

void findPattern(const std::string& name);

/* ************************************* */

int main() try {
    std::cout << "Enter the name of input points file: \n";
    std::string s = "largeMystery.txt";
    //std::cin >> s;  // e.g. points1.txt, points200.txt, largeMystery.txt
    
    findPattern(s);
    plotData(s);
} catch (const std::exception& e) {
    fmt::print("Error: {}", e.what());
    return 1;
}

/* ************************************* */

void plotData(const std::string& name) {
    std::filesystem::path points_name = name;
    const auto points = readPoints(data_dir / points_name);

    std::filesystem::path segments_name = "segments-" + name;
    const auto lines = readLineSegments(data_dir / "output" / segments_name);

    rendering::Window window(850, 850, rendering::Window::UseVSync::Yes);
    while (!window.shouldClose()) {
        window.beginFrame();
        window.clear({0, 0, 0, 1});
        window.drawLines(lines);    // to plot the line segments discovered
        window.drawPoints(points);  // to plot input points
        window.endFrame();
    }
}

void findPattern(const std::string& name) {
    std::filesystem::path points_name = name;
    const auto points = readPoints(data_dir / points_name);

    std::set<std::pair<int, std::set<std::pair<int, int>>>> lines;

    size_t allowedSize = 0;
    for(int i = 0;i < std::ssize(points) - 1;i++)
    {
        std::unordered_map<float, std::set<std::pair<int, int>>> slopes;

        for(int j = i + 1;j < std::ssize(points);j++)
        {
            float X1 = points[i].position.x * 32767;
            float Y1 = points[i].position.y * 32767;
            float X2 = points[j].position.x * 32767;
            float Y2 = points[j].position.y * 32767;

            float key;
            if(X1 != X2)
                key = (Y2 - Y1)/(X2 - X1);
            else
                key = std::numeric_limits<float>::infinity();

            //slopes[key].insert({ X1 * 32767, Y1 * 32767 });
            //slopes[key].insert({ X2 * 32767, Y2 * 32767 });
            if(i != j)
            {
                if(key == -0)
                    key = 0;
                slopes[key].insert({ X1, Y1 });
                slopes[key].insert({ X2, Y2 });
            }

            //float key = (points[j].position.y - points[i].position.y)/(points[j].position.x - points[i].position.x);

            //if(i != j && key >= 0)
            //{
            //    key = std::abs(key);
            //    slopes[key].insert({ points[j].position.x * 32767, points[j].position.y * 32767 });
            //    slopes[key].insert({ points[i].position.x * 32767, points[i].position.y * 32767 });
            //}
        }

        for(auto& e : slopes)
        {
            if(e.second.size() > 3)
            {
                std::cout << "Current size: " << e.second.size() << "\tAllowed size: " << allowedSize << "\n";
                if(e.second.size() >= allowedSize)
                {
                    lines.insert({ (*(e.second.begin())).second, e.second });
                    allowedSize = e.second.size();
                    std::cout << "Slope: " << e.first << "\n";
                    for(auto i : e.second)
                    {
                        std::cout << "X: " << i.first << "\tY: " << i.second << "\n";
                    }
                    std::cout << "\n";
                }
                else
                {
                    std::cout << "Decrease allowed size\n\n";
                    allowedSize--;
                }
            }
        }
    }

    std::filesystem::path segments_name = "segments-" + name;
    std::ofstream output;
    output.open(data_dir / "output" / segments_name);

    for(auto i : lines)
    {
        std::pair<int, int> p1 = *(i.second.begin());
        std::pair<int, int> p2 = *(--i.second.end());
        output << p1.first << " " << p1.second << " " << p2.first << " " << p2.second << "\n";

        for(auto j : i.second)
        {
            if(j != *(--i.second.end()))
                std::cout << "(" << j.first << "," << j.second << ")->";
            else
                std::cout << "(" << j.first << "," << j.second << ")\n";
        }
    }

    output.close();
}
