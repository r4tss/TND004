#include <linesdiscoverysystem/readfiles.h>

#include <vector>
#include <string>
#include <filesystem>

#include <rendering/window.h>
#include <fmt/format.h>

#include <unordered_map>
#include <set>
#include <fstream>

void plotData(const std::string& name);

void findSegments(const std::string& name);

/* ************************************* */

int main() try {
    std::cout << "Enter the name of input points file: \n";
    std::string s;
    std::cin >> s;  // e.g. points1.txt, points200.txt, largeMystery.txt
    
    findSegments(s);
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

void findSegments(const std::string& name) {
    std::filesystem::path points_name = name;
    const auto points = readPoints(data_dir / points_name);

    std::set<std::pair<int, std::set<std::pair<int, int>>>> lines;
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
            
            if(i != j)
            {
                if(key == -0)
                    key = 0;
                slopes[key].insert({ X1, Y1 });
                slopes[key].insert({ X2, Y2 });
            }
        }

        for(auto& e : slopes)
        {
            if(e.second.size() > 3)
            {
                lines.insert({ (*(e.second.begin())).second, e.second });
            }
        }
    }

    // Remove short segments
    for(auto i = lines.begin();i != lines.end();++i)
    {
        for(auto j = lines.begin();j != lines.end();)
        {
            if(i != j)
            {
                std::pair<int, int> p11 = *((*i).second.begin());
                std::pair<int, int> p12 = *(--(*i).second.end());
                std::pair<int, int> p21 = *((*j).second.begin());
                std::pair<int, int> p22 = *(--(*j).second.end());

                int slope1, slope2;
                if(p11.first != p12.first)
                    slope1 = (p12.second - p11.second)/(p12.first - p11.first);
                else
                    slope1 = 2147483647;

                if(p21.first != p22.first)
                    slope2 = (p22.second - p21.second)/(p22.first - p21.first);
                else
                    slope2 = 2147483647;
                
                if(slope1 == slope2 && p11.first >= p21.first && p11.second >= p21.second && p12.first <= p22.first && p12.second <= p22.second)
                {
                    i = lines.erase(i);
                    if(i == lines.end())
                        j = lines.end();
                    else
                        j = lines.begin();
                }
                else
                    ++j;
            }
            else
                ++j;
        }
    }

    // Print segments to std::cout and output file
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
