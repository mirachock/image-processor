#include "args.h"
#include "controller.h"
#include "factory.h"
#include "formats/bmp.h"

#include <iostream>
#include <string_view>
#include <vector>

void Reference() {
    std::cout << "Incorrect launch. Description of the command-line argument format:\n\n"
                 "{program name} {input file path} {output file path} "
                 "[-{filter name 1} [filter parameter 1] [filter parameter 2] ...] "
                 "[-{filter name 2} [filter parameter 1] [filter parameter 2] ...] ...:\n\n"
                 "Supported filters:\n\n";
    for (const std::string_view f : factory::KnownTypes()) {
        std::cout << factory::FilterInfo(f) << '\n';
    }
    std::cout << std::endl;
}

int main(int argc, char** argv) {
    try {
        if (argc == 1) {
            Reference();
            return 0;
        }

        const std::vector<std::string_view> raw_args(argv + 1, argv + argc);
        const args::Args args = args::ParseArgs(raw_args);
        controller::Controller controller(args.filters);

        image::Image img = bmp::Read(args.input);
        img = controller.Apply(std::move(img));
        bmp::Write(args.output, img);
    } catch (const error::Error& error) {
        std::cerr << error.Name() << ": " << error.what() << '\n';
        std::cerr << "Help information is displayed if the program is launched without arguments." << std::endl;
        return 1;
    } catch (const error::Defect& error) {
        std::cerr << error.Name() << ": " << error.what() << '\n';
        std::cerr << "Author is bad." << std::endl;
        return 2;
    } catch (const std::exception& e) {
        std::cerr << "Are you sure you want to do this?\n" << e.what() << std::endl;
        return 3;
    }
}
