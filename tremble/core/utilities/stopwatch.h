#pragma once

/**
* @brief class, made for measuring passed time
*/
class Stopwatch
{
public:
    Stopwatch(std::string stopwatch_name = std::string());
    void Reset(); //!< Set the timer to 0
    /**
    * @brief print the result and reset the timer
    * @param identifier prints identifier along with the result
    */
    void PrintAndReset(char* identifier = nullptr); //!< 
    float OutputAndReset(); //!< Output the result of the function in seconds and reset the timer
    void Print(char* identifier = nullptr); //!< Print the current time on the stopwatch
    float Output(); //!< Output the current time on the stopwatch

private:
    std::string name_;
    unsigned long long ticks_per_second_;
    unsigned long long start_; //!< Time during last reset
    double elapsed;
};