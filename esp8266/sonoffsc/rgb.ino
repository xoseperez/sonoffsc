
// This function 
void sendColor(char * rgbValues) {

	int rValue = 0;
	int gValue = 0;
	int bValue = 0;

	// split the data into its parts

	char * strtokIndx; // this is used by strtok() as an index

	strtokIndx = strtok(rgbValues, ",");	// get the first part - the string
	rValue = atoi(strtokIndx);				// convert this part to an integer

	strtokIndx = strtok(NULL, ",");			// this continues where the previous call left off
	gValue = atoi(strtokIndx);				// convert this part to an integer

	strtokIndx = strtok(NULL, ",");
	bValue = atoi(strtokIndx);				// convert this part to an integer
	
	commSendRGB(rValue, gValue, bValue);  // Send it to the Atmega328
}