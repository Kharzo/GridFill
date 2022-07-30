/*
 Name: Joseph Kharzo
 Date: 6/14/2022
 Description: The user chooses the dimensions of a grid. Then, the user chooses a starting location and a
	      goal on the grid. After that, the program populates the grid with numbers that increase from
	      the chosen location, and then a path is created from the goal to the starting location.
 Usage: The user is prompted to enter the dimensions of the grid, the starting location, and the goal.
*/

// The required libraries

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// The required functions
void inputCheck();
void printEnvironment();
void floodFill();
void floodFillInverse();
void mooreNeighborhood();
char** userGridCreate();
void userGridFill();

int main(int argc, char** argv){
	
	// Declaring useful variables
	
	srand(time(0));
	int width;
	int height;
	int impassibleTerrain;
	int amountImpassible;
	int goalPosX;
	int goalPosY;
	int startPosX;
	int startPosY;
	char cont = 'n';
	int keepRunning = 1;

	printf("%s\n\n", "Welcome to Joseph's CSE240 WaveFront Pather!");
	
	// The user is prompted for the width and height of the grid	
	inputCheck("Enter the width (at least 10): ", "Please enter a value greater than or equal to 10.", &width, 10, 0);
	inputCheck("Enter the height (at least 10): ", "Please enter a value greater than or equal to 10.", &height, 10, 0);
	

	// Creating the main array
	const int SIZE = width * height;
	
	int* grid[height];
	
	// The memory of the array is allocated row by row	
	int i;
	for(i = 0; i < height; i++){
		grid[i] = (int*)malloc(width*sizeof(int));
	}
	
	// The main array is populated
	int j;
	for(i = 0; i < height; i++){
		for(j = 0; j < width; j++){
			grid[i][j] = 0;
		}
	}
	
	
	// The user is prompted for the number of obstacles on the grid
	// The prompt is in a loop to check if the user has entered a number greater than 35
	// This loop also allows the user to change their mind and enter a lower number
	do{
		inputCheck("Enter the percent of impassible terrain (0-100): ", "Please enter a value between 0 and 100.", &impassibleTerrain, 0, 0);
		
		if(impassibleTerrain > 35){
			printf("Having a value greater than 35 may create poor results, do you wish to continue? (y/n): ");
			scanf(" %c", &cont);	
		}else{
			keepRunning = 0;
		}

		if(cont == 'y'){
			keepRunning = 0;
		}
	}while(keepRunning);
	
	// The amount of obstacles to enter into the grid
	amountImpassible = (SIZE * impassibleTerrain) / 100;
	
	// This enters the obstacles into the array
	// The loop runs for the number of obstacles
	// the rand function is seeded and produces a random number from 0 to the max size of the grid
	for(i = 0; i < amountImpassible; i++){
		grid[rand() % height][rand() % width] = -1;
	}

	// The user is prompted to enter the goal
	// This is checked against the obstacles and a reprompt is given	
	keepRunning = 1;
	do{
		inputCheck("Please enter the Goal Position X: ","Sorry, that position is not valid.", &goalPosX, width, 1);
		inputCheck("Please enter the Goal Position Y: ","Sorry, that position is not valid.", &goalPosY, height, 1);
		
		if(grid[goalPosY][goalPosX] == -1){
			printf("Sorry, that position is inside an obstacle.\n");
		}else{
			keepRunning = 0;
		}
		
	}while(keepRunning);
	
	// The user is prompted to enter the starting position
	// If the position is in an obstacle, then the loop will continue running and the user will be prompted again
	keepRunning = 1;
	do{
		inputCheck("Please enter the Start Position X: ","Sorry, that position is not valid.", &startPosX, width, 1);
		inputCheck("Please enter the Start Position Y: ","Sorry, that position is not valid.", &startPosY, height, 1);
		
		if(grid[startPosY][startPosX] == -1){
			printf("Sorry, that position is inside an obstacle.\n");
		}else{
			keepRunning = 0;
		}
		
	}while(keepRunning);
	
	grid[startPosY][startPosX] = 1; // The starting position is placed on the grid
	
	// The grid is populated with numbers
	floodFill(grid, width, height, startPosX, startPosY);
	
	// The grid is shown to the user
	printEnvironment(grid, width, height);
	
	// The array that the user will be shown in the end is declared
	char* userGrid[SIZE];
	
	// Memory is allocated
	for(i = 0; i < height; i++){
		userGrid[i] = (char*)malloc(width * sizeof(char));
	}
	
	// The final grid is filled with whitespace
	for(i = 0; i < height; i++){
		for(j = 0; j < width; j++){
			
			userGrid[i][j] = ' ';
			
		}
	}

	// The start position on the new grid is entered
	userGrid[goalPosY][goalPosX] = '@';
	
//	printf("Grid created\n");
	
	// The second grid is filled
	userGridFill(grid, userGrid, width-1, height-1, goalPosX, goalPosY);
	
	// The goal is entered on the grid
	userGrid[startPosY][startPosX] = '$';
	
	// The final array is shown to the user
	for(i = 0; i < height; i++){
		for(j = 0; j < width; j++){
			printf("%c\t", userGrid[i][j]);
		}
		printf("\n");
	}
	
	// The memory that was allocated is freed
	// This is done for both arrays
	for(i = 0; i < height; i++){
		free(grid[i]);
		free(userGrid[i]);
	}
}

// This function is where the final array is created and filled
void userGridFill(int** grid, char** userGrid, int width, int height, int startX, int startY){
	int r = startY;
	int c = startX;
	
	int center = grid[r][c];
	
	// These set of if-else statements check for the position of the cell that is being used to start the pathfinding
	// These are possible cases for the grid; the top left, top center, top right, left without top or bottom, the center without top and bottom rows, bottom row without corners, the right without corners
	// bottom right, bottom left.
	
	if(c < width && c > 0&& center != -1 && center != 0){ // the central area
//		printf("Entered 1\n");
		if(r < height && r > 0){ // in the direct central area
//			printf("Entered 1.1\n");
			if(center > grid[r][c+1] && grid[r][c+1] != -1){ // the center value is checked against the next square. 
									// The cell is checked for an obstacle
									// if there is no obstacle, then a hashtag is placed in the final array that is shown to the user
									// Then, the column is incremented
									// Each check has the same idea except they check a different area
				userGrid[r][c+1] = '#';
				c++;
			}else if(center > grid[r-1][c+1]&& grid[r-1][c+1] != -1){
				userGrid[r-1][c+1] = '#';
				r--;
				c++;
			}else if(center > grid[r-1][c]&& grid[r-1][c] != -1){
				userGrid[r-1][c] = '#';
				r--;
			}else if(center > grid[r-1][c-1]&& grid[r-1][c-1] != -1){
				userGrid[r-1][c-1] = '#';
				r--;
				c--;
			}else if(center > grid[r][c-1]&& grid[r][c-1] != -1){
				userGrid[r][c-1] = '#';
				c--;
			}else if(center > grid[r+1][c-1]&& grid[r+1][c-1] != -1){
				userGrid[r+1][c-1] = '#';
				r++;
				c--;
			}else if(center > grid[r+1][c]&& grid[r+1][c] != -1){
				userGrid[r+1][c] = '#';
				r++;
			}else if(center > grid[r+1][c+1]&& grid[r+1][c+1] != -1){
				userGrid[r+1][c+1] = '#';
				r++;
				c++;
			}		
		}else if(r == 0){ // Center top
//			printf("Entered 1.2\n");
			if(center > grid[r][c+1]&& grid[r][c+1] != -1){
				userGrid[r][c+1] = '#';
				c++;
			}else if(center > grid[r][c-1]&& grid[r][c-1] != -1){
				userGrid[r][c-1] = '#';
				c--;
			}else if(center > grid[r+1][c-1]&& grid[r+1][c-1] != -1){
				userGrid[r+1][c-1] = '#';
				r++;
				c--;
			}else if(center > grid[r+1][c]&& grid[r+1][c] != -1){
				userGrid[r+1][c] = '#';
				r++;
			}else if(center > grid[r+1][c+1]&& grid[r+1][c+1] != -1){
				userGrid[r+1][c+1] = '#';
				r++;
				c++;
			}
		}else if(r == height){ // Center top
//			printf("Entered 1.3\n");
			if(center > grid[r][c+1]&& grid[r][c+1] != -1){
				userGrid[r][c+1] = '#';
				c++;
			}else if(center > grid[r-1][c+1]&& grid[r-1][c+1] != -1){
				userGrid[r-1][c+1] = '#';
				r--;
				c++;
			}else if(center > grid[r-1][c]&& grid[r-1][c] != -1){
				userGrid[r-1][c] = '#';
				r--;
			}else if(center > grid[r-1][c-1]&& grid[r-1][c-1] != -1){
				userGrid[r-1][c-1] = '#';
				r--;
				c--;
			}else if(center > grid[r][c-1]&& grid[r][c-1] != -1){
				userGrid[r][c-1] = '#';
				c--;
			}
		}
	}else if(c ==0&& center != -1 && center != 0){ // The left side
//		printf("Entered 2\n");
		if(r < height && r > 0){ // Center
//			printf("Entered 2.1\n");
			if(center > grid[r][c+1]&& grid[r][c+1] != -1){
				userGrid[r][c+1] = '#';
				c++;
			}else if(center > grid[r-1][c+1]&& grid[r-1][c+1] != -1){
				userGrid[r-1][c+1] = '#';
				r--;
				c++;
			}else if(center > grid[r-1][c]&& grid[r-1][c] != -1){
				userGrid[r-1][c] = '#';
				r--;
			}else if(center > grid[r+1][c]&& grid[r+1][c] != -1){
				userGrid[r+1][c] = '#';
				r++;
			}else if(center > grid[r+1][c+1]&& grid[r+1][c+1] != -1){
				userGrid[r+1][c+1] = '#';
				r++;
				c++;
			}		
		}else if(r == 0){ // The top
//			printf("Entered 2.2\n");
			if(center > grid[r][c+1]&& grid[r][c+1] != -1){
				userGrid[r][c+1] = '#';
				c++;
			}else if(center > grid[r+1][c]&& grid[r+1][c] != -1){
				userGrid[r+1][c] = '#';
				r++;
			}else if(center > grid[r+1][c+1]&& grid[r+1][c+1] != -1){
				userGrid[r+1][c+1] = '#';
				r++;
				c++;
			}
		}else if(r == height){ // The bottom
//			printf("Entered 2.3\n");
			if(center > grid[r][c+1]&& grid[r][c+1] != -1){
				userGrid[r][c+1] = '#';
				c++;
			}else if(center > grid[r-1][c+1]&& grid[r-1][c+1] != -1){
				userGrid[r-1][c+1] = '#';
				r--;
				c++;
			}else if(center > grid[r-1][c]&& grid[r-1][c] != -1){
				userGrid[r-1][c] = '#';
				r--;
			}
		}
	}else if(c == width && center != -1 && center != 0){ // The right side
//		printf("Entered 3\n");
		if(r < height && r > 0){
//			printf("Entered 3.1\n");
			if(center > grid[r-1][c]&& grid[r-1][c] != -1){
				userGrid[r-1][c] = '#';
				r--;
			}else if(center > grid[r-1][c-1]&& grid[r-1][c-1] != -1){
				userGrid[r-1][c-1] = '#';
				r--;
				c--;
			}else if(center > grid[r][c-1]&& grid[r][c-1] != -1){
				userGrid[r][c-1] = '#';
				c--;
			}else if(center > grid[r+1][c-1]&& grid[r+1][c-1] != -1){
				userGrid[r+1][c-1] = '#';
				r++;
				c--;
			}else if(center > grid[r+1][c]&& grid[r+1][c] != -1){
				userGrid[r+1][c] = '#';
				r++;
			}		
		}else if(r == 0){
//			printf("Entered 3.2\n");
		 	if(center > grid[r][c-1]&& grid[r][c-1] != -1){
				userGrid[r][c-1] = '#';
				c--;
			}else if(center > grid[r+1][c-1]&& grid[r+1][c-1] != -1){
				userGrid[r+1][c-1] = '#';
				r++;
				c--;
			}else if(center > grid[r+1][c]&& grid[r+1][c] != -1){
				userGrid[r+1][c] = '#';
				r++;
			}
		}else if(r == height){
			//printf("Entered 3.3\n");
			if(center > grid[r-1][c]&& grid[r-1][c] != -1){
				userGrid[r-1][c] = '#';
				r--;
			}else if(center > grid[r-1][c-1]&& grid[r-1][c-1] != -1){
				userGrid[r-1][c-1] = '#';
				r--;
				c--;
			}else if(center > grid[r][c-1]&& grid[r][c-1] != -1){
				userGrid[r][c-1] = '#';
				c--;
			}
		}
	}
	
	// This makes the function recursive
	// If the row or column are changed and the center value does not equal to one, then this function is called again
		
	if(r == startY && c == startX && center != 1){
//		printf("A path could not be found.\n");
	}else if(center ==1){
		
	}else{
		userGridFill(grid, userGrid,width, height, c, r);
	}
	
}

// Checks the user's input
void inputCheck(const char* message, const char* errorMessage, int* waveFrontSetting, int condition, int positionCheck){
		
	int keepRunning = 1;
	
	
	while(keepRunning){
		// The input check is standard
		// This checks if the program is checking for a position or the sizr of the array
		// The function also prints an error message depending on what is being checked
		printf("%s", message);
		scanf("%d", waveFrontSetting);
		if( !positionCheck && *waveFrontSetting >= condition && *waveFrontSetting <= 100){
			keepRunning = 0;
		}else if(positionCheck && *waveFrontSetting < condition && *waveFrontSetting >= 0 ){
			keepRunning = 0;
		}else{
			printf("%s\n",errorMessage);
		}
	}	
}


// This function prints the integer array to the user using a nested for loop
void printEnvironment(int** array, int width, int height){
	
	int i, j;
	for(i = 0; i < height; i++){
		for(j = 0; j < width; j++){
			printf("%d\t", array[i][j]);
		}
		printf("\n");
	}	
	
}

// This is a recursive function that fills the array with numbers
void floodFill(int** array, int width, int height, int startX, int startY){
	//printf("Entered function\n");
	int c = startX;
	int r = startY;
	
	// If the column is not at the end of the array and the height is not at the bottom
	// Then the square is appended and the next column is checked
	// If the final column is reached, then the row is appended and the column is reset
	// If both conditions have been met, then the function is no longer executed
	if(c < width){
	//	printf("Passed width check: %d\n", c);
		if(r < height){
	//		printf("Passed height check: %d\n",r);
			mooreNeighborhood(array, width, height, c, r);
			floodFill(array, width, height, c+1, r);		
		}	
	}else{
	//	printf("Failed width check: %d\n", c);
		if(r < height){
	//		printf("Passed second height check: %d\n", r);
			mooreNeighborhood(array,width,height,c,r);
			floodFill(array, width, height, 0, r+1);
		}	
	}
	//printf("Outside the first function\n");
	
	// An inverse of this function is called
	floodFillInverse(array, width, height, startX, startY-1);
}

// This recursive function moves the opposite way to the first floodFill()
void floodFillInverse(int** array, int width, int height, int startX, int startY){
//	printf("Entered function\n");
	int c = startX;
	int r = startY;
	
	// Insted of incrementing, this decrements the rows and columns
	// The inverse is done from the first function
	// This checks if column is at 0 and if row is at 0
	if(c > 0){
//		printf("Passed width check second: %d\n", c);
		if(r > 0){
//			printf("Passed height check second: %d\n",r);
			mooreNeighborhood(array, width, height, c, r);
			floodFillInverse(array, width, height, c-1, r);		
		}	
	}else{
//		printf("Failed width check: %d\n", c);
		if(r > 0){
//			printf("Passed second height check second: %d\n", r);
			mooreNeighborhood(array,width,height,c,r);
			floodFillInverse(array, width, height, width-1, r-1);
		}	
	}
//	printf("Outside the second function\n");
	
}

// This function fills the grid with number, and it is only called by the floodFill recursive function
void mooreNeighborhood(int** array, int width, int height,int centerX, int centerY, int gridOrUserGrid){
	
//	printf("Entered Moore\n");	
	int c = centerX;
	int r = centerY;
	
	int center = array[r][c];
		
	height = height - 1;
		
	// This function operates the same as the userGridFill array if-else statements
	// Except the cell in the array is incremented accordingly
	if(r != 0 && c != 0&& r != height && c != width){ // The center
//		printf("Entered 1\n");	
		if(array[r+1][c] <  center && array[r+1][c] != -1 && array[r+1][c] == 0){
			array[r+1][c] = center + 1;
		}
		if(array[r][c+1] <  center && array[r][c+1] != -1&& array[r][c+1] == 0){
			array[r][c+1] = center + 1;
		}
		if(array[r-1][c] <  center&& array[r-1][c] != -1&& array[r-1][c] == 0){
			array[r-1][c] = center + 1;
		}
		if(array[r][c-1] <  center&& array[r][c-1] != -1&& array[r][c-1] == 0){
			array[r][c-1] = center + 1;
		}
		if(array[r+1][c+1] <  center&& array[r+1][c+1] != -1&& array[r+1][c+1] == 0){
			array[r+1][c+1] = center + 1;
		}
		if(array[r-1][c-1] <  center&& array[r-1][c-1] != -1&& array[r-1][c-1] == 0){
			array[r-1][c-1] = center + 1;
		}
		if(array[r+1][c-1] <  center&& array[r+1][c-1] != -1&& array[r+1][c-1] == 0){
			array[r+1][c-1] = center + 1;
		}
		if(array[r-1][c+1] <  center&& array[r-1][c+1] != -1&& array[r-1][c+1] == 0){
			array[r-1][c+1] = center + 1;
		}
	}else if(r != 0 && c == 0 && r != height){ // The left
//		printf("Entered 2\n");
		if(array[r+1][c] <  center&& array[r+1][c] != -1&& array[r+1][c] == 0){
			array[r+1][c] = center + 1;
		}
		if(array[r][c+1] <  center&& array[r][c+1] != -1&& array[r][c+1] == 0){
			array[r][c+1] = center + 1;
		}
		if(array[r-1][c] <  center&& array[r-1][c] != -1&& array[r-1][c] == 0){
			array[r-1][c] = center + 1;
		}
		if(array[r+1][c+1] <  center&& array[r+1][c+1] != -1&& array[r+1][c+1] == 0){
			array[r+1][c+1] = center + 1;
		}
		if(array[r-1][c+1] <  center&& array[r-1][c+1] != -1&& array[r-1][c+1] == 0){
			array[r-1][c+1] = center + 1;
		}
	}else if(r == 0 && c != 0&& c != width){ // The top
//		printf("Entered 3\n");
		if(array[r+1][c] <  center && array[r+1][c] != -1&& array[r+1][c] == 0 ){
			array[r+1][c] = center + 1;
		}
		if(array[r][c+1] <  center && array[r][c+1] != -1&& array[r][c+1] == 0){
			array[r][c+1] = center + 1;
		}
		if(array[r][c-1] <  center&& array[r][c-1] != -1&& array[r][c-1] == 0){
			array[r][c-1] = center + 1;
		}
		if(array[r+1][c+1] <  center&& array[r+1][c+1] != -1&& array[r+1][c+1] == 0){
			array[r+1][c+1] = center + 1;
		}
		if(array[r+1][c-1] <  center&& array[r+1][c-1] != -1&& array[r+1][c-1] == 0){
			array[r+1][c-1] = center + 1;
		}
	}else if(r== 0 && c == 0){// Top left etc.
//		printf("Entered 4\n");
		if(array[r+1][c] <  center && array[r+1][c] != -1&& array[r+1][c] == 0 ){
			array[r+1][c] = center + 1;
		}
		if(array[r][c+1] <  center && array[r][c+1] != -1&& array[r][c+1] == 0){
			array[r][c+1] = center + 1;
		}
		if(array[r+1][c+1] <  center&& array[r+1][c+1] != -1&& array[r+1][c+1] == 0){
			array[r+1][c+1] = center + 1;
		}
	}else if(r == 0 && c == width){
//		printf("Entered 5\n");
		if(array[r+1][c] <  center && array[r+1][c] != -1 && array[r+1][c] == 0){
			array[r+1][c] = center + 1;
		}
		if(array[r][c-1] <  center&& array[r][c-1] != -1&& array[r][c-1] == 0){
			array[r][c-1] = center + 1;
		}
		if(array[r+1][c-1] <  center&& array[r+1][c-1] != -1&& array[r+1][c-1] == 0){
			array[r+1][c-1] = center + 1;
		}
	}else if(r == height && c == 0){
//		printf("Entered 6\n");	
		if(array[r][c+1] <  center && array[r][c+1] != -1&& array[r][c+1] == 0){
			array[r][c+1] = center + 1;
		}
		if(array[r-1][c] <  center&& array[r-1][c] != -1&& array[r-1][c] == 0){
			array[r-1][c] = center + 1;
		}
		if(array[r-1][c+1] <  center&& array[r-1][c+1] != -1&& array[r-1][c+1] == 0){
			array[r-1][c+1] = center + 1;
		}
	}else if(r==height&& c== width){
//		printf("Entered 7\n");
		if(array[r-1][c] <  center&& array[r-1][c] != -1&& array[r-1][c] == 0){
			array[r-1][c] = center + 1;
		}
		if(array[r][c-1] <  center&& array[r][c-1] != -1&& array[r][c-1] == 0){
			array[r][c-1] = center + 1;
		}
		if(array[r-1][c-1] <  center&& array[r-1][c-1] != -1&& array[r-1][c-1] == 0){
			array[r-1][c-1] = center + 1;
		}
	}else if(r==height&& c!=0 && c!= width){
//		printf("Entered 8\n");
		if(array[r][c+1] <  center && array[r][c+1] != -1&& array[r][c+1] == 0){
			array[r][c+1] = center + 1;
		}
		if(array[r-1][c] <  center&& array[r-1][c] != -1&& array[r-1][c] == 0){
			array[r-1][c] = center + 1;
		}
		if(array[r][c-1] <  center&& array[r][c-1] != -1&& array[r][c-1] == 0){
			array[r][c-1] = center + 1;
		}
		if(array[r-1][c-1] <  center&& array[r-1][c-1] != -1&& array[r-1][c-1] == 0){
			array[r-1][c-1] = center + 1;
		}
		if(array[r-1][c+1] <  center&& array[r-1][c+1] != -1&& array[r-1][c+1] == 0){
			array[r-1][c+1] = center + 1;
		}
	}else if(r != 0 && r!= height && c == width){
//		printf("Entered 9\n");
		if(array[r+1][c] <  center && array[r+1][c] != -1&& array[r+1][c] == 0 ){
			array[r+1][c] = center + 1;
		}
		if(array[r-1][c] <  center&& array[r-1][c] != -1&& array[r-1][c] == 0){
			array[r-1][c] = center + 1;
		}
		if(array[r][c-1] <  center&& array[r][c-1] != -1&& array[r][c-1] == 0){
			array[r][c-1] = center + 1;
		}
		if(array[r-1][c-1] <  center&& array[r-1][c-1] != -1&& array[r-1][c-1] == 0){
			array[r-1][c-1] = center + 1;
		}
		if(array[r+1][c-1] <  center&& array[r+1][c-1] != -1&& array[r+1][c-1] == 0){
			array[r+1][c-1] = center + 1;
		}
	}
	
//	printf("Left Moore\n");
}
