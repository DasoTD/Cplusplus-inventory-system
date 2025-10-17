#include <drogon/drogon.h>
#include <iostream>

using namespace drogon;

int main() {
    // Start the drogon client-only event loop
    app().run();
    return 0;
}