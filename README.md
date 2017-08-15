# Adafruit_FreeTouch

This is a QTouch-like library allowing use of the PTC (peripheral touch controller) on the ATSAMD11 and ATSAMD21.

This code is a FORK of the adafruit library with the following major changes:

- Improved internal documentation
- Removal of reliance on the ASF framework (and the Adafruit_ASFCore library) which opens the door for drop-in compatability with SAMD variants beyond the 21G
- Added support for the SAMD11 architecture which is a cheaper chip with more available Y sense lines than the SAMD21



