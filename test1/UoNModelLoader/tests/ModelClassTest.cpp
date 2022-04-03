/**
 * @file ModelClassTest.cpp
 * @author Christodoulos Sotiriou
 * @brief Test code for the Model class
 * @version 0.1
 * @date 2021-11-15
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <filesystem>
#include <iostream>

#include <Model.hpp>

int main() {
    /* Create Model Instance */
    Model model;
    int res;

    std::cout << "[i] Current Path: " << std::filesystem::current_path()
              << std::endl;

    /* Parsing Non-Existant File */
    res = model.parseFile("does-not-exist");
    if (res != Model::ERR_FILE_NOT_FOUND) {
        std::cout
            << "[-] <does-not-exist> should have thrown FILE_NOT_FOUND error\n";
        return 1;
    }

    /* Parsing Malformed Model File */
    res = model.parseFile("TestModels/MalformedModel.mod");
    if (res != Model::ERR_SYNTAX_ERROR) {
        std::cout << "[-] <TestModels/MalformedModel.mod> should have thrown "
                     "SYNTAX_ERROR\n";
        return 1;
    }
    if (model.printVertexCount() != 0) {
        std::cout << "No Model Loaded" << std::endl;
        return 1;
    }

    /* Parsing File */
    res = model.parseFile("TestModels/ExampleModel3.mod");
    if (res != Model::ERR_NONE) {
        std::cout
            << "[-] <TestModels/ExampleModel2.mod> Found Error in valid file\n";
        return 1;
    }

    if (model.printVertexCount() != 220) {
        std::cout << "Model Has not Been Loaded correctly" << std::endl;
        return 1;
    }
    Vector center = model.getModelCenter();
    /* We can't really compare doubles easilly for exact value so I'm leaving
     * this as it is for now */
    if (center.x == 0 && center.y == 0 && center.z == 0) {
        std::cout << "Something went wrong in center calculation" << std::endl;
        return 1;
    }

    model.printCellInfo();

    /* Model Saving */
    res = model.saveModel("ExampleModel3_copy.mod");
    if (res != Model::ERR_NONE) {
        std::cout << "Something went wrong when saving" << std::endl;
        return 1;
    }

    return 0;
}
