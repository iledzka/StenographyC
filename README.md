# StenographyC
A simple program that can be used do encode and decode text message inside a PPM image.

The task was to implement a program that can produce a plain PPM file with encoded text message. To do this, user must enter a secret, which is an integer with up to 20 digits. The program can also decode the message, given that the encoded PPM message and correct secret are provided.

The requirements included hiding the message by using LSB (Least Significant Bit) technique. The program must also work on images with arbitrary number of rows and columns. The image data must be read into memory; this include a dynamic number of comments.

