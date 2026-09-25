#include "vk_config.hpp"

int main ()
{
    try
    {
        Sirius app;
        app.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}