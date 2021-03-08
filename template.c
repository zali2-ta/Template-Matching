/*****
--- Created By: Ali, Zeenat
--- Project#1: Template Matching using pixelwise brute force
--- Term: CMSC 691 Spring 2021
*****/

#include <stdio.h>
#include <stdlib.h>
#include<math.h>
#include<limits.h>
#include<string.h>
#include "dc_image.h"

#define MIN(a,b)  ( (a) < (b) ? (a) : (b) )
#define MAX(a,b)  ( (a) > (b) ? (a) : (b) )
#define ABS(x)    ( (x) <= 0 ? 0-(x) : (x) )

/*--- Define Point ---*/
struct Point {
	int x, y;
};

/*--- Draw Point ---*/
//void DrawPoint(int y, int x, int size, byte*** img) {
//	int minx, miny, maxx, maxy;
//	int rows, cols;
//	minx = x - size;
//	maxx = x + size;
//	miny = y - size;
//	maxy = y + size;
//	for (int y = miny; y < maxy; y++) {
//		for (int x = minx; x < maxx; x++) {
//			img[y][x][0] = 255;
//			img[y][x][1] = 0;
//			img[y][x][2] = 255;
//		}
//	}
//}

void DrawPoint(int y, int x, int size, byte*** img, int irows, int icols, int theta, int scale) {
	//printf("theta : %d", theta);
	int minx, miny, maxx, maxy;
	int rotx = 0, roty = 0;
	minx = x - size;
	maxx = x + size;
	miny = y - size;
	maxy = y + size;
	for (int y = miny; y < maxy; y++) {
		for (int x = minx; x < maxx; x++) {
			//rotx = ((x * cos((double)theta)) - (y * sin((double)theta)));
			//roty = ((x * sin((double)theta)) + (y * cos((double)theta)));
			rotx = MIN(MAX(((scale * x * cos((double)theta)) - (y * sin((double)theta))), 5), icols - 5);
			roty = MIN(MAX(((scale * x * sin((double)theta)) + (y * cos((double)theta))), 5), irows - 5);
			//printf("rotx, roty : %d, %d", rotx, roty);
			img[roty][rotx][0] = 255;
			img[roty][rotx][1] = 0;
			img[roty][rotx][2] = 255;
		}
	}
}


/*--- Draw Line ---*/
void DrawLine(struct Point A, struct Point B, int size, byte*** image, int irows, int icols, int theta, int scale)
{
	struct Point M; // midpoint
	M.x = (int)(A.x + B.x) / 2;
	M.y = (int)(A.y + B.y) / 2;
	if (((ABS(M.x - A.x) <= size) && (ABS(M.y - A.y) <= size)) || ((ABS(M.x - B.x) <= size) && (ABS(M.y - B.y) <= size))) {
		DrawPoint(M.y, M.x, size, image, irows, icols, theta, scale);
	}
	else {
		DrawLine(A, M, size, image, irows, icols, theta, scale); // start to midpoint
		DrawLine(M, B, size, image, irows, icols, theta, scale); // midpoint to end
	}
}
/*--- Flip Image ---*/
byte*** FlipImage(byte*** image, int rows, int cols) {
	printf("\nFlipping the image...\n");
	byte*** rotImage = malloc3d(rows, cols, 3);
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			rotImage[rows - 1 - y][x][0] = image[y][x][0];
			rotImage[rows - 1 - y][x][1] = image[y][x][1];
			rotImage[rows - 1 - y][x][2] = image[y][x][2];
		}
	}
	//SaveRgbPng(rotImage, "out/flippedImage.png", rows, cols);
	return rotImage;
}


/*--- Rotate Image ---*/
void RotateImage(byte*** img, int irows, int icols, byte*** temp, int trows, int tcols, int  theta) {
	int y, x; // iterator variables
	int off_x = 100, off_y = 200; // starting point (origin) of temp in img
	double ux = cos(theta), uy = sin(theta), vx = -uy, vy = ux; // vector components
	printf("\n theta, ux, uy: %lf %lf %lf\n", theta, ux, uy);
	int rotx, roty; // rotated x and y
	printf("Testing.1,2,3.");

	// Rotating the temp window on image
	printf("Rotating the temp window on image...");
	//byte*** rotwinImg = malloc3d(irows, icols, 3);
	for (y = 0; y < trows; y++) {
		for (x = 0; x < tcols; x++) {
			rotx = off_x + (x * ux) + (y * vx);
			roty = off_y + (x * uy) + (y * vy);
			//printf("\n rotx and roty: %d %d", rotx, roty);
			if (roty >= 0 && roty < irows && rotx >= 0 && rotx < icols) {
				//DrawPoint(roty, rotx, 1, img, theta);
				img[roty][rotx][0] = 255;
				img[roty][rotx][1] = 0;
				img[roty][rotx][2] = 255;
			}
		}
	}
	SaveRgbPng(img, "out/rotatedImg.png", irows, icols);
}

/*--- Draw Match ---*/
void DrawMatch(byte*** img, int irows, int icols, int trows, int tcols, int size, int y, int x, int theta, int scale) {
	struct Point P, Q, R, S;
	//Draw matched window on image
	//double ux = scale*cos((double)theta), uy = scale * sin((double)theta), vx = -uy, vy = ux; // vector components
	int best_x = x; //(x * ux) + (y * vx);
	int best_y = y; // (x * uy) + (y * vy);

	P.x = MIN(MAX(best_x, 0), icols - 1);  //best_x - tcols / 2;
	P.y = MIN(MAX(best_y, 0), irows - 1);  //best_y - trows / 2;
	Q.x = MIN(MAX(best_x, 0), icols - 1); // best_x - tcols / 2;
	Q.y = MIN(MAX(best_y + trows, 0), irows - 1); // best_y + trows / 2;
	R.x = MIN(MAX(best_x + tcols, 0), icols - 1); // best_x + tcols / 2;
	R.y = MIN(MAX(best_y + trows, 0), irows - 1); // best_y + trows / 2;
	S.x = MIN(MAX(best_x + tcols, 0), icols - 1); // best_x + tcols / 2;
	S.y = MIN(MAX(best_y, 0), irows - 1); // best_y - trows / 2;
	//printf("Draw match: Points assigned");
	DrawLine(P, Q, size, img, irows, icols, theta, scale);
	DrawLine(Q, R, size, img, irows, icols, theta, scale);
	DrawLine(R, S, size, img, irows, icols, theta, scale);
	DrawLine(S, P, size, img, irows, icols, theta, scale);
}

/*--- Template Matching using Brute Force ---*/
void TemplateMatch(byte*** img, int irows, int icols, byte*** temp, int trows, int tcols, int size) {
	int x, y;// iterator variables
	int off_y, off_x; //offsets in image
	int x_i, y_i; //new coord in image
	int best_x = 0, best_y = 0;
	int best_r = 0, best_s = 1; //best parameters
	//double current_Loss, best_loss = 10000000000.0;
	int current_Loss, best_loss = 1000000000;
	int rotx, roty; // rotated x and y

	for (off_y = 0; off_y < irows; off_y += 20) {
		for (off_x = 0; off_x < icols; off_x += 20) {
			for (int r = 0; r < 360; r += 45) {
				for (double s = 0.5; s < 2.1; s += 0.5) {
					//RotateImage(img, irows, icols, temp, trows, tcols, r);
					double ux = s * cos((double)r), uy = s * sin((double)r), vx = -uy, vy = ux; // vector components
					current_Loss = 0;
					for (y = 0; y < trows; y += 1) {
						for (x = 0; x < tcols; x += 1) {
							//rotate coord

							rotx = MIN(MAX(((x * ux) + (y * vx)), 0), tcols - 1);
							roty = MIN(MAX(((x * uy) + (y * vy)), 0), trows - 1);

							x_i = MIN(MAX(off_x + rotx, 0), icols - 1);
							y_i = MIN(MAX(off_y + roty, 0), irows - 1);
							//Loss function
							current_Loss += ABS(img[y_i][x_i][0] - temp[y][x][0]) + ABS(img[y_i][x_i][1] - temp[y][x][1]) + ABS(img[y_i][x_i][2] - temp[y][x][2]);
						}
					}
					if (current_Loss < best_loss) {
						best_loss = current_Loss;
						best_y = off_y;
						best_x = off_x;
						best_r = r;
						best_s = s;
						printf("\n Best Loss: %d \n", best_loss);
						printf("best_y, best_x, rotx, roty, best_r: %d %d %d %d %d", best_y, best_x, rotx, roty, best_r);
					}
				}
			}
		}
	}
	printf("\n Best Loss: done\n");
	printf("best_y, best_x: %d %d ", best_y, best_x);
	printf("best_r: %d ", best_r);
	//DrawPoint(best_y, best_x, 10, img, irows, icols, best_r);
	DrawMatch(img, irows, icols, trows, tcols, size, best_y, best_x, best_r, best_s);
	SaveRgbPng(img, "out/matchedPointdraw.png", irows, icols);
}


/*--- Scale Image ---*/
byte*** ScaleImage(byte*** image, int rows, int cols, double scale) {
	int y, x, y2, x2;
	int newRows, newCols;
	newRows = rows - (scale * rows);
	newCols = cols - (scale * cols);
	printf("\n scale: %f \n", scale);
	printf("\n input rows: %d \n", rows);
	printf("\n new rows: %f \n", newRows);
	for (y = 0; y < rows; y++)
	{
		y2 = (newRows * y) / rows;
		for (x = 0; x < cols; x++)
		{
			x2 = (newCols * x) / cols;
			image[y2][x2][0] = image[y][x][0];
			image[y2][x2][1] = image[y][x][1];
			image[y2][x2][2] = image[y][x][2];
		}
	}

	SaveRgbPng(image, "out/ScaledImage.png", newRows, newCols);
	return image;
}

/*--- Main Driving Function ---*/
int main(int argc, char* argv[])
{
	if (argc == 3) {
		printf("Image name is: %s \n", argv[1]);
		printf("Template name is: %s \n", argv[2]);
	}
	else if (argc > 3) {
		printf("Too many arguments supplied.\n");
		return 0;
	}
	else {
		printf("Two arguments expected: Image name and Template name.\n");
		return 0;
	}
	int y, x, size = 1; // size of pixel to draw
	struct Point A, B;
	int irows, icols, ichan;
	int trows, tcols, tchan;

	// Read the image in img [y][x][c]   y number rows   x cols  c 3
	byte*** img = LoadRgb(argv[1], &irows, &icols, &ichan);
	printf("Image: img %p rows %d cols %d chan %d\n", img, irows, icols, ichan);

	// Read the template in temp [y][x][c]   y number rows   x cols  c 3
	byte*** temp = LoadRgb(argv[2], &trows, &tcols, &tchan);
	printf("Template: img %p rows %d cols %d chan %d\n", temp, trows, tcols, tchan);

	//Save loaded image
	SaveRgbPng(img, "out/1_img.png", irows, icols);
	SaveRgbPng(temp, "out/1_temp.png", trows, tcols);

	// Flip the image
	img = FlipImage(img, irows, icols);
	SaveRgbPng(img, "out/flipped_Img.png", irows, icols);
	temp = FlipImage(temp, trows, tcols);
	SaveRgbPng(temp, "out/flipped_Temp.png", trows, tcols);

	TemplateMatch(img, irows, icols, temp, trows, tcols, size);

	char outputtemp[20];
	char output_filename[20];
	strncpy(outputtemp, argv[2], strlen(argv[2]));
	outputtemp[strlen(argv[2])] = 0;
	printf(outputtemp);

	if (strcmp(outputtemp, "puppy_template1.jpg") == 0) {
		strncpy(output_filename, "puppy_match1.jpg", 20);
	}
	else if (strcmp(outputtemp, "puppy_template2.jpg") == 0) {
		strncpy(output_filename, "puppy_match2.jpg", 20);
	}
	else if (strcmp(outputtemp, "puppy_template3.jpg") == 0) {
		strncpy(output_filename, "puppy_match3.jpg", 20);
	}
	else if (strcmp(outputtemp, "houses_template1.jpg") == 0) {
		strncpy(output_filename, "houses_match1.jpg", 20);
	}
	else if (strcmp(outputtemp, "houses_template2.jpg") == 0) {
		strncpy(output_filename, "houses_match2.jpg", 20);
	}
	else if (strcmp(outputtemp, "houses_template3.jpg") == 0) {
		strncpy(output_filename, "houses_match3.jpg", 20);
	}
	else if (strcmp(outputtemp, "tiger_template1.jpg") == 0) {
		strncpy(output_filename, "tiger_match1.jpg", 20);
	}
	else if (strcmp(outputtemp, "tiger_template2.jpg") == 0) {
		strncpy(output_filename, "tiger_match2.jpg", 20);
	}
	else if (strcmp(outputtemp, "tiger_template3.jpg") == 0) {
		strncpy(output_filename, "tiger_match3.jpg", 20);
	}
	else {
		printf("Incorrect arguments given");
		return 0;
	}
	
	//Flip the image back
	img = FlipImage(img, irows, icols);
	SaveRgbPng(img, output_filename, irows, icols);
	//temp = FlipImage(temp, trows, tcols);
	//SaveRgbPng(temp, "out/Output_Temp.png", trows, tcols);

	printf("Done!\n");
	return 0;
}