 /*

 ####################################################################################
 #  BlackLib Library controls Beaglebone Black's inputs and outputs.                #
 #  Copyright (C) 2013-2015 by Yigit YUCE                                           #
 ####################################################################################
 #                                                                                  #
 #  This file is part of BlackLib library.                                          #
 #                                                                                  #
 #  BlackLib library is free software: you can redistribute it and/or modify        #
 #  it under the terms of the GNU Lesser General Public License as published by     #
 #  the Free Software Foundation, either version 3 of the License, or               #
 #  (at your option) any later version.                                             #
 #                                                                                  #
 #  BlackLib library is distributed in the hope that it will be useful,             #
 #  but WITHOUT ANY WARRANTY; without even the implied warranty of                  #
 #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                   #
 #  GNU Lesser General Public License for more details.                             #
 #                                                                                  #
 #  You should have received a copy of the GNU Lesser General Public License        #
 #  along with this program.  If not, see <http://www.gnu.org/licenses/>.           #
 #                                                                                  #
 #  For any comment or suggestion please contact the creator of BlackLib Library    #
 #  at ygtyce@gmail.com                                                             #
 #                                                                                  #
 ####################################################################################

 */



#include "BlackPWM.h"
#include "dirent.h"
#include "string.h"

#include "fstream"
#include "stdio.h"

namespace BlackLib
{

const BlackPWM::Chip_Channel_List_t BlackPWM::Chip_Channel_List[PwmChannels_Amount] = {
            		{ 0, '0' },
					{ 0, '1' },
					{ 1, '0' },
					{ 1, '1' }
            };


    // ########################################### BLACKPWM DEFINITION STARTS ############################################ //
    BlackPWM::BlackPWM(pwmName pwm)
    {

    	 this->pwmErrors = new errorPWM();

    	if(pwm < PwmChannels_Amount) {

			this->ChipPath = "/sys/class/pwm/pwmchip" + std::to_string(Chip_Channel_List[pwm].ChipNum);

			DIR * Direction = opendir(this->ChipPath.c_str());

			if(Direction != NULL) {

				closedir(Direction);

				std::ofstream WriteFileStream;

				WriteFileStream.open((this->ChipPath + "/export").c_str(),std::ios::out);

				if(WriteFileStream.is_open() == true) {

					WriteFileStream << Chip_Channel_List[pwm].ChannelName << std::endl;

					WriteFileStream.close();

		            this->periodPath = this->ChipPath + "/pwm" + Chip_Channel_List[pwm].ChannelName + "/period";
		            this->dutyPath = this->ChipPath + "/pwm" + Chip_Channel_List[pwm].ChannelName + "/duty_cycle";
		            this->runPath = this->ChipPath + "/pwm" + Chip_Channel_List[pwm].ChannelName + "/enable";
		            this->polarityPath = this->ChipPath + "/pwm" + Chip_Channel_List[pwm].ChannelName + "/polarity";

		            this->pwm_Num = pwm;

				} else {

					pwmErrors->initError = true;

				}

			} else {

				pwmErrors->initError = true;

			}

    	} else {

    		this->pwmErrors->initError = true;

    	}

    }

    BlackPWM::~BlackPWM()
    {

    	if(pwmErrors->initError == true) {

    		std::ofstream WriteFileStream;

    		WriteFileStream.open((this->ChipPath + "/unexport").c_str(),std::ios::out);

    		if(WriteFileStream.is_open() == true) {

    			WriteFileStream << Chip_Channel_List[pwm_Num].ChannelName << std::endl;

    			WriteFileStream.close();

    		}

    	}

        delete this->pwmErrors;
    }

    std::string BlackPWM::getValue()
    {
        double period   = static_cast<long double>( this->getNumericPeriodValue() );
        double duty     = static_cast<long double>( this->getNumericDutyValue() );

        return tostr(static_cast<float>( (1.0 - (duty / period )) * 100 ));
    }

    std::string BlackPWM::getPeriodValue()
    {
        std::ifstream periodValueFile;

        periodValueFile.open(this->periodPath.c_str(),std::ios::in);
        if(periodValueFile.fail())
        {
            periodValueFile.close();
            this->pwmErrors->periodFileError = true;
            return FILE_COULD_NOT_OPEN_STRING;
        }
        else
        {
            std::string readValue;
            periodValueFile >> readValue;

            periodValueFile.close();
            this->pwmErrors->periodFileError = false;
            return readValue;
        }
    }

std::string BlackPWM::getDutyValue() {
	std::ifstream dutyValueFile;

	dutyValueFile.open(this->dutyPath.c_str(), std::ios::in);

	if (dutyValueFile.fail()) {

		dutyValueFile.close();
		this->pwmErrors->dutyFileError = true;
		return FILE_COULD_NOT_OPEN_STRING;

	} else {

		std::string readValue;
		dutyValueFile >> readValue;

		dutyValueFile.close();
		this->pwmErrors->dutyFileError = false;
		return readValue;

	}
}

std::string BlackPWM::getRunValue() {
	std::ifstream runValueFile;

	runValueFile.open(this->runPath.c_str(), std::ios::in);
	if (runValueFile.fail()) {
		runValueFile.close();
		this->pwmErrors->runFileError = true;
		return FILE_COULD_NOT_OPEN_STRING;
	} else {
		std::string readValue;
		runValueFile >> readValue;

		runValueFile.close();
		this->pwmErrors->runFileError = false;
		return readValue;
	}
}

std::string BlackPWM::getPolarityValue() {
	std::ifstream polarityValueFile;

	polarityValueFile.open(this->polarityPath.c_str(), std::ios::in);
	if (polarityValueFile.fail()) {
		polarityValueFile.close();
		this->pwmErrors->polarityFileError = true;
		return FILE_COULD_NOT_OPEN_STRING;
	} else {
		std::string readValue;
		polarityValueFile >> readValue;

		polarityValueFile.close();
		this->pwmErrors->polarityFileError = false;
		return readValue;
	}
}

float BlackPWM::getNumericValue() {
	double period = static_cast<long double>(this->getNumericPeriodValue());
	double duty = static_cast<long double>(this->getNumericDutyValue());

	return static_cast<float>((1.0 - (duty / period)) * 100);
}

inline int64_t BlackPWM::getNumericPeriodValue() {
	int64_t readValue = FILE_COULD_NOT_OPEN_INT;

	std::ifstream periodValueFile;

	periodValueFile.open(this->periodPath.c_str(), std::ios::in);
	if (periodValueFile.fail()) {
		periodValueFile.close();
		this->pwmErrors->periodFileError = true;
	} else {
		periodValueFile >> readValue;

		periodValueFile.close();
		this->pwmErrors->periodFileError = false;
	}
	return readValue;
}

inline int64_t BlackPWM::getNumericDutyValue() {
	int64_t readValue = FILE_COULD_NOT_OPEN_INT;
	std::ifstream dutyValueFile;

	dutyValueFile.open(this->dutyPath.c_str(), std::ios::in);
	if (dutyValueFile.fail()) {
		dutyValueFile.close();
		this->pwmErrors->dutyFileError = true;
	} else {
		dutyValueFile >> readValue;

		dutyValueFile.close();
		this->pwmErrors->dutyFileError = false;
	}
	return readValue;
}

bool BlackPWM::setDutyPercent(float percantage) {
	if (percantage > 100.0 or percantage < 0.0) {
		this->pwmErrors->outOfRange = true;
		this->pwmErrors->dutyFileError = true;
		this->pwmErrors->periodFileError = true;
		return false;
	}

	this->pwmErrors->outOfRange = false;

	std::ofstream dutyFile;
	dutyFile.open(this->dutyPath.c_str(), std::ios::out);
	if (dutyFile.fail()) {
		dutyFile.close();
		this->pwmErrors->dutyFileError = true;
		return false;
	} else {
		dutyFile
				<< static_cast<int64_t>(std::round(
						(this->getNumericPeriodValue())
								* (percantage / 100)));
		dutyFile.close();
		this->pwmErrors->dutyFileError = false;
		return true;
	}

}

bool BlackPWM::setPeriodTime(uint64_t period, timeType tType) {
	uint64_t writeThis = static_cast<uint64_t>(period
			* static_cast<double>(pow(10, static_cast<int>(tType) + 9)));

	if (writeThis > 1000000000) {
		this->pwmErrors->outOfRange = true;
		return false;
	} else {
		this->pwmErrors->outOfRange = false;
		std::ofstream periodFile;
		periodFile.open(this->periodPath.c_str(), std::ios::out);
		if (periodFile.fail()) {
			periodFile.close();
			this->pwmErrors->periodFileError = true;
			return false;
		} else {
			periodFile << writeThis;
			periodFile.close();
			this->pwmErrors->periodFileError = false;
			return true;
		}
	}

}

bool BlackPWM::setSpaceRatioTime(uint64_t space, timeType tType) {

	uint64_t writeThis = static_cast<int64_t>(space	* static_cast<double>(pow(10, static_cast<int>(tType) + 9)));

	if (writeThis > 1000000000) {

		this->pwmErrors->outOfRange = true;

		return false;

	} else {

		std::ofstream dutyFile;

		dutyFile.open(this->dutyPath.c_str(), std::ios::out);

		if (dutyFile.fail()) {

			dutyFile.close();

			this->pwmErrors->dutyFileError = true;

			return false;

		} else {

			dutyFile << writeThis;

			dutyFile.close();

			this->pwmErrors->dutyFileError = false;

			return true;

		}

	}
}

bool BlackPWM::setLoadRatioTime(uint64_t load, timeType tType) {

	uint64_t writeThis = (this->getNumericPeriodValue()	- static_cast<int64_t>(load	* static_cast<double>(pow(10, static_cast<int>(tType) + 9))));

	if (writeThis > 1000000000) {

		this->pwmErrors->outOfRange = true;

		return false;

	} else {
		std::ofstream dutyFile;

		dutyFile.open(this->dutyPath.c_str(), std::ios::out);

		if (dutyFile.fail()) {

			dutyFile.close();

			this->pwmErrors->dutyFileError = true;

			return false;

		} else {

			dutyFile << writeThis;

			dutyFile.close();

			this->pwmErrors->dutyFileError = false;

			return true;

		}
	}
}

bool BlackPWM::setPolarity(polarityType polarity) {
	std::ofstream polarityFile;

	polarityFile.open(this->polarityPath.c_str(), std::ios::out);

	if (polarityFile.fail()) {

		polarityFile.close();

		this->pwmErrors->polarityFileError = true;

		return false;

	} else {

		polarityFile << static_cast<int>(polarity);

		polarityFile.close();

		this->pwmErrors->polarityFileError = false;

		return true;

	}

}

bool BlackPWM::setRunState(runValue state) {

	std::ofstream runFile;

	runFile.open(this->runPath.c_str(), std::ios::out);

	if (runFile.fail()) {

		runFile.close();

		this->pwmErrors->runFileError = true;

		return false;

	} else {

		runFile << static_cast<int>(state);

		runFile.close();

		this->pwmErrors->runFileError = false;

		return true;

	}

}

bool BlackPWM::isRunning() {
	return (this->getRunValue() == "1");
}

bool BlackPWM::isPolarityStraight() {

	return !(this->getPolarityValue() == "1");

}

bool BlackPWM::isPolarityReverse() {

	return (this->getPolarityValue() == "1");

}

void BlackPWM::toggleRunState() {

	if (this->getRunValue() == "1") {

		this->setRunState(stop);

	} else {

		this->setRunState(run);

	}

}

void BlackPWM::tooglePolarity() {

	if (this->getPolarityValue() == "0") {

		this->setPolarity(LOW);

	} else {

		this->setPolarity(HIGH);

	}

}

bool BlackPWM::fail() {

	return (this->pwmErrors->outOfRange
			or this->pwmErrors->runFileError
			or this->pwmErrors->dutyFileError
			or this->pwmErrors->periodFileError
			or this->pwmErrors->polarityFileError
			or this->pwmErrors->initError);
}

bool BlackPWM::fail(BlackPWM::flags f) {

	if (f == outOfRangeErr) {

		return this->pwmErrors->outOfRange;

	}

	if (f == runFileErr) {

		return this->pwmErrors->runFileError;

	}

	if (f == dutyFileErr) {

		return this->pwmErrors->dutyFileError;

	}

	if (f == periodFileErr) {

		return this->pwmErrors->periodFileError;

	}

	if (f == polarityFileErr) {

		return this->pwmErrors->polarityFileError;

	}

	if (f == InitializingError) {

		return this->pwmErrors->initError;

	}

	return true;
}

// ########################################### BLACKPWM DEFINITION STARTS ############################################ //

} /* namespace BlackLib */
