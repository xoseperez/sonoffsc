
// Code borrowed from the Adafruit Neopixel Library.  Idea is to send colors as single value that can be decoded at other end
uint8_t rValue = 0;
uint8_t gValue = 0;
uint8_t bValue = 0;

// Convert separate R,G,B into packed 32-bit RGB color.
// Packed format is always RGB, regardless of LED strand color order.
uint32_t getRgbColor(uint8_t r, uint8_t g, uint8_t b) {
	return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

// Convert separate R,G,B,W into packed 32-bit WRGB color.
// Packed format is always WRGB, regardless of LED strand color order.
uint32_t getRgbColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
	return ((uint32_t)w << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

uint16_t rgbBlue() {
	getRgbColor(0, 0, 255);
}


// Function to return uint32t values from component values
uint32_t parseColor(char * rgbValues) {

	// split the data into its parts

	char * strtokIndx; // this is used by strtok() as an index

	strtokIndx = strtok(rgbValues, ",");	// get the first part - the string
	rValue = atoi(strtokIndx);				// convert this part to an integer

	strtokIndx = strtok(NULL, ",");			// this continues where the previous call left off
	gValue = atoi(strtokIndx);				// convert this part to an integer

	strtokIndx = strtok(NULL, ",");
	bValue = atoi(strtokIndx);				// convert this part to an integer
	return getRgbColor(rValue, gValue, bValue);
}