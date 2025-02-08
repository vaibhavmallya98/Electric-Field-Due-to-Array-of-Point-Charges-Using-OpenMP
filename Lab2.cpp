/*
Author: Bantwal Vaibhav Mallya 
Class: ECE 6122 A
Last Date Modified: 8th October 2023

Description:
This file computes the electric field at a location due to a 2D array of point charges using OpenMP 

*/

#include <iostream>
#include<math.h>
#include "omp.h"
#include<iomanip>
#include<chrono>
using namespace std;

//this function prompts the user for a new location and calculates the electric field at a new location 
void promptFunction(int numThreads,int nRows,int nColumns,double xSeparation,double ySeparation,double pCharge){
    double xLoc,yLoc,zLoc; //x, y and z coordinates of point charge
    double const k = 9.0 * pow(10,9);//coulumbs constant 
    double elecFieldX=0, elecFieldY=0, elecFieldZ=0;

    string promptUser; //asks user if electric field is to be calculated at a new location

    cout<<"Please enter the location in space to determine the electric field (x y z) in meters: ";
    cin>>xLoc>>yLoc>>zLoc;

    bool invalidPointFlag = false;

    while(!invalidPointFlag){
        for(int i=0;i<nRows;++i){
            for(int j=0;j<nColumns;++j){
                double xCharge = (double)((double)i - (double)(nColumns-1)/2)*xSeparation;
                double yCharge = (double)((double)j - (double)(nRows-1)/2)*ySeparation;
                double zCharge = 0;
                
                if(xLoc == xCharge && yLoc == yCharge && zLoc == zCharge){
                    cout<<"\nInvalid point location!!";
                    cout<<"Please enter the location in space to determine the electric field (x y z) in meters: ";
                    cin>>xLoc>>yLoc>>zLoc;
                    break;
                }
            }
            if(invalidPointFlag){
                break;
            }
        }
        invalidPointFlag = true; 

    }

    auto startTime = std::chrono::high_resolution_clock::now();

    omp_set_num_threads(numThreads);

    //using openmp to calculate electric field at a location 
    #pragma omp parallel for reduction(+:elecFieldX,elecFieldY,elecFieldZ) schedule(static)
    for(int i=0;i<nRows;++i){
        for(int j=0;j<nColumns;++j){
            double xHat = xLoc-(double)((double)i - (double)(nColumns-1)/2)*xSeparation;
            double yHat = yLoc-(double)((double)j - (double)(nRows-1)/2)*ySeparation;
            double zHat = zLoc;
            double r = xHat * xHat + yHat*yHat + zHat*zHat;
            double rCube = r*sqrt(r);
            double numerator = k*pCharge;
            elecFieldX += (numerator/rCube) * xHat;
            elecFieldY += (numerator/rCube) * yHat;
            elecFieldZ += (numerator/rCube) * zHat;

        }
    }

    auto stopTime = std::chrono::high_resolution_clock::now();
    
    //time difference between before calculation starts and after calculation is done 
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stopTime - startTime);

    //finding the decimal (mantissa) and power of 10 (exponent) of the calculated electric fields in x, y and z directions
    int exponentEfx = static_cast<int>(std::log10(std::abs(elecFieldX)));  
    double efxMantissa = elecFieldX/pow(10,exponentEfx);

    int exponentEfy = static_cast<int>(std::log10(std::abs(elecFieldY)));
    double efyMantissa = elecFieldY/pow(10,exponentEfy);

    int exponentEfz = static_cast<int>(std::log10(std::abs(elecFieldZ)));
    double efzMantissa = elecFieldZ/pow(10,exponentEfz);

    std::cout << std::fixed << std::setprecision(4);

    cout<<"\nElectric field at ("<<xLoc<<","<<yLoc<<","<<zLoc<<") in V/m is";

    cout<<"\nEx = "<<efxMantissa<<" * 10^"<<exponentEfx;
    cout<<"\nEy = "<<efyMantissa<<" * 10^"<<exponentEfy;
    cout<<"\nEz = "<<efzMantissa<<" * 10^"<<exponentEfz;

    double totalEf = sqrt(pow(elecFieldX,2) + pow(elecFieldY,2) + pow(elecFieldZ,2));
    int exponentTotalEf = static_cast<int>(std::log10(std::abs(totalEf)));
    double totalEfMantissa = totalEf/pow(10,exponentTotalEf);

    cout<<"\n|E| = "<<totalEfMantissa<<" * 10^"<<exponentTotalEf; //calculating resultant electric field strength 
    
    cout<<"\nThe calculation took "<<duration.count()<<" microsec!";

    //prompting the user for a new location 
    cout<<"\nDo you want to enter a new location (Y/N)?";
    cin>>promptUser;

    //recursive function that prompts the user for a new location and calculates electric field at the new location
    if(promptUser == "Y" || promptUser == "y"){
        promptFunction(numThreads,nRows,nColumns,xSeparation,ySeparation,pCharge);
    }
    else{
        cout<<"\nBye!";
    }
}

int main(){
    int numThreads,nRows,nColumns;//number of threads, charge array rows, charge array columns 

    double xSeparation, ySeparation, pCharge; //x and y separation distances and point charge value 

    double xLoc, yLoc, zLoc; //location coordinates where electric field is to be calculated 

    double const k = 9.0 * pow(10,9); //coulumbs constant 

    double elecFieldX=0, elecFieldY=0, elecFieldZ=0; //x, y and z electric field values initialized to 0 

    string promptUser;//checks if the user wants to enter the location again 

    cout<<"Please enter the number of concurrent threads to use: ";
    cin>>numThreads;

    cout<<"\nPlease enter the number of rows and columns in the NxM array: ";

    //checking if rows and columns of charge array are natural numbers 
    do {
        if (std::cin >>nRows>>nColumns && nRows>0 && nColumns>0) {
            
            break;  
        } else {
            
            std::cin.clear();  
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            std::cout << "Both N and M should be natural numbers!!!\n";
            cout<<"\nPlease enter the number of rows and columns in the N x M array: ";
        }
    } while (true);

    cout<<"Please enter the x and y separation distances in meters: ";

    //checking if x and y separation distances are >0.0 and valid numerical values
    do {
        if (std::cin>>xSeparation>>ySeparation && xSeparation>0.0 && ySeparation>0.0) {
            
            break;  
        } else {
            
            std::cin.clear();  
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            std::cout << "Invalid input. Both x and y separation distances should be >0.0 and valid numerical values\n";
            cout<<"\nPlease enter the x and y separation distance in meters: ";
        }
    } while (true);

     
    cout<<"Please enter the common charge on the points in micro C: ";
    //checking if the charge value is a valid numerical value
    do {
        
        if (std::cin >> pCharge) {
            break;  
        } else {
            
            std::cin.clear();  
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            std::cout << "Invalid input. The charge value should be a numerical value\n";
            cout<<"\nPlease enter the common charge on the points in micro C: ";
        }
    } while (true);

    cout<<"Please enter the location in space to determine the electric field (x y z) in meters: ";
    cin>>xLoc>>yLoc>>zLoc;

    
    bool invalidPointFlag = false;
    
    //checking if the entered location is same as the charge location
    while(!invalidPointFlag){
        for(int i=0;i<nRows;++i){
            for(int j=0;j<nColumns;++j){
                double xCharge = (double)((double)i - (double)(nColumns-1)/2)*xSeparation;
                double yCharge = (double)((double)j - (double)(nRows-1)/2)*ySeparation;
                double zCharge = 0;
                
                if(xLoc == xCharge && yLoc == yCharge && zLoc == zCharge){
                    cout<<"\nInvalid point location!!";
                    cout<<"Please enter the location in space to determine the electric field (x y z) in meters: ";
                    cin>>xLoc>>yLoc>>zLoc;
                    break;
                }
            }
            if(invalidPointFlag){
                break;
            }
        }
        invalidPointFlag = true; 

    }

    
    pCharge = pCharge * pow(10,-6); //making charge into microcoulumbs 

    auto startTime = std::chrono::high_resolution_clock::now();

    omp_set_num_threads(numThreads); //setting the number of threads to the entered value 

    //using openmp to compute electric field at an entered location
    #pragma omp parallel for reduction(+:elecFieldX,elecFieldY,elecFieldZ) schedule(static)
    
    for(int i=0;i<nRows;++i){
        for(int j=0;j<nColumns;++j){
            double xHat = xLoc-(double)((double)i - (double)(nColumns-1)/2)*xSeparation;
            double yHat = yLoc-(double)((double)j - (double)(nRows-1)/2)*ySeparation;
            double zHat = zLoc;
            double r = xHat * xHat + yHat*yHat + zHat*zHat;
            double rCube = r*sqrt(r);
            double numerator = k*pCharge;
            elecFieldX += (numerator/rCube) * xHat;
            elecFieldY += (numerator/rCube) * yHat;
            elecFieldZ += (numerator/rCube) * zHat;
        }
    }
    
    

    auto stopTime = std::chrono::high_resolution_clock::now();

    //time difference between before calculation starts and after calculation is done 
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stopTime - startTime);

    //finding the decimal (mantissa) and power of 10 (exponent) of the calculated electric fields in x, y and z directions 
    int exponentEfx = static_cast<int>(std::log10(std::abs(elecFieldX)));
    double efxMantissa = elecFieldX/pow(10,exponentEfx);

    int exponentEfy = static_cast<int>(std::log10(std::abs(elecFieldY)));
    double efyMantissa = elecFieldY/pow(10,exponentEfy);

    int exponentEfz = static_cast<int>(std::log10(std::abs(elecFieldZ)));
    double efzMantissa = elecFieldZ/pow(10,exponentEfz);

    std::cout << std::fixed << std::setprecision(4);

    cout<<"\nElectric field at ("<<xLoc<<","<<yLoc<<","<<zLoc<<") in V/m is";

    cout<<"\nEx = "<<efxMantissa<<" * 10^"<<exponentEfx;
    cout<<"\nEy = "<<efyMantissa<<" * 10^"<<exponentEfy;
    cout<<"\nEz = "<<efzMantissa<<" * 10^"<<exponentEfz;

    double totalEf = sqrt(pow(elecFieldX,2) + pow(elecFieldY,2) + pow(elecFieldZ,2));
    int exponentTotalEf = static_cast<int>(std::log10(std::abs(totalEf)));
    double totalEfMantissa = totalEf/pow(10,exponentTotalEf);

    cout<<"\n|E| = "<<totalEfMantissa<<" * 10^"<<exponentTotalEf;
    
    cout<<"\nThe calculation took "<<duration.count()<<" microsec!";

    //prompting the user for a new location
    cout<<"\nDo you want to enter a new location (Y/N)?";
    cin>>promptUser;

    if(promptUser == "Y" || promptUser == "y"){
        //recursive function that prompts user for a new location and calculates electric field at the new location
        promptFunction(numThreads,nRows,nColumns,xSeparation,ySeparation,pCharge);
    }
    else{
        cout<<"\nBye!";
    }

}

