
#include <iostream>

void clear_screen()
{
// CSI[2J clears screen, CSI[H moves the cursor to top-left corner
std::cout << "\x1B[2J\x1B[H";
}