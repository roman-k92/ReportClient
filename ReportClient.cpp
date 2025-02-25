#include <iostream>
#include "CreateReport.h"

int main()
{
    CreateReport *rp = new CreateReport();
    StatusConnection status;

    while(true)
    {
        status = rp->sendRequest();

        if (status != StatusConnection::SUCCESS)
        {
            std::cout << "There is an error during request!" << std::endl;
            break;
        }
        system("cls");
    }

    delete rp;

    return 0;
}