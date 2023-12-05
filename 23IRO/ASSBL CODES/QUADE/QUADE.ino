char mystr[5];
bool inPatch;
int inc = 0;
int current_destination = 0;
int n_destinations = 0; 
#include <List.hpp>

#include "I2Cdev.h"

#include "MPU6050_6Axis_MotionApps20.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 mpu;
//MPU6050 mpu(0x69); // <-- use for AD0 high

#define OUTPUT_READABLE_YAWPITCHROLL

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };



// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================


//notes to self
// /10 is for x values
// /%10 is for y values
//Checklist:
// have a way to have multiple goals
// make the path take the one with teh least amount of turns
// calibration sequence for omni

#define MD1 4
#define MD2 7
#define MD3 8
#define MD4 11

#define MS1 5
#define MS2 6
#define MS3 9
#define MS4 10

int speed = 150;
bool process = false;

#define DIAL 14

#define S1 2
#define S2 15
#define S3 16
#define S4 17

List<int> all_f_costs;
List<int> all_h_costs;
List<char> path;
List<int> goals;
List<char> goal_type;
List<int> placeholder;
//rows then columns y, x
const int x = 7;
const int y = 7;

// DataInitiator
#define SIG 12

// Sound Sensors
#define SX1 13
#define SX2 3

// Set Offset for higher accuracy in sampling (nSec * 1000)
#define nOffSet 220  // 0.15 : 250 | 0.1 : 220
// baudRate for receiving the datas (nSec * 1000)
#define nBaudRate 165 //  0.15 : 190 | 0.1 : 165

// 7 by 7 grid
int nGridRow = 7;
int nGridColumn = 7;

// preventing reading SIG more than once
bool bSIG_OT = false;

// repeating var for printing nodes
int nRepreat = 0;

// Data Initiate Approval
bool bReadData = false;

// printNode? true/false
bool bApprovePrintNode = false;

int nGrid[7][7] = {{0,0,0,0,2,0,0},
                   {0,2,2,0,2,0,0},
                   {0,2,0,0,2,0,0},
                   {0,2,2,0,2,0,0},
                   {0,0,0,0,2,0,0},
                   {2,2,2,2,2,0,0},
                   {0,0,0,0,0,0,0}};// 1- start, 2-obstacle, 3-destination

char cGrid[7][7] = { { 'o', 'o', 'o', 'o', 'o', 'o', 'o' },
                     { 'o', 'o', 'o', 'o', 'o', 'o', 'o' },
                     { 'o', 'o', 'o', 'o', 'o', 'o', 'o' },
                     { 'o', 'o', 'o', 'o', 'o', 'o', 'o' },
                     { 'o', 'o', 'o', 'o', 'o', 'o', 'o' },
                     { 'o', 'o', 'o', 'o', 'o', 'o', 'o' },
                     { 'o', 'o', 'o', 'o', 'o', 'o', 'o' } };

int Coordinates[7][7][5]= {}; // nGrid for naviagtion
                           // type of tile, where the node came from , if it has been visited (parented?) , gcost,hcost , change in direction
bool onetime = true;
int goal = 22;
int start = goal;
int location = start;
int current_location = start;
int parent_node;
int child_nodes[4] = {0,0,0,0};
bool pathmade = true;

void setup() {
  goals.addLast(30);
  goals.addLast(22);
  goals.addLast(33);
  goal_type.addLast('P');
  goal_type.addLast('D');
  goal_type.addLast('N');
  Serial.begin(115200);

  pinMode(MD1 , OUTPUT);
  pinMode(MD2 , OUTPUT);
  pinMode(MD3 , OUTPUT);
  pinMode(MD4 , OUTPUT);

  pinMode(S1 , INPUT);
  pinMode(S2 , INPUT);
  pinMode(S3 , INPUT);
  pinMode(S4 , INPUT);

  pinMode(SX1 , INPUT);
  pinMode(SX2 , INPUT);
  pinMode(SIG , INPUT);

  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
      Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
  #endif

  while (!Serial); // wait for Leonardo enumeration, others continue immediately

  Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();

  // verify connection
  Serial.println(F("Testing device connections..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

  // load and configure the DMP
  Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
      // Calibration Time: generate offsets and calibrate our MPU6050
      mpu.CalibrateAccel(6);
      mpu.CalibrateGyro(6);
      mpu.PrintActiveOffsets();
      // turn on the DMP, now that it's ready
      Serial.println(F("Enabling DMP..."));
      mpu.setDMPEnabled(true);
      mpuIntStatus = mpu.getIntStatus();
      dmpReady = true;

      // get expected DMP packet size for later comparison
      packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
      // ERROR!
      // 1 = initial memory load failed
      // 2 = DMP configuration updates failed
      // (if it's going to break, usually the code will be 1)
      Serial.print(F("DMP Initialization failed (code "));
      Serial.print(devStatus);
      Serial.println(F(")"));
  }
  setSpeed();
}

int g_cost(int coord){
  //return abs((parent_node % 10) - (start % 10)) + abs((parent_node / 10) - (start / 10)) + abs((coord % 10) - (parent_node % 10)) + abs((coord / 10) - (parent_node / 10));
  int a = 0;
  
  for(int i = parent_node ; Coordinates[i%10][i/10][0] != 1 ;){
    i = Coordinates[i%10][i/10][1];
    a++;
  }
  return a + abs((coord % 10) - (parent_node % 10)) + abs((coord / 10) - (parent_node / 10));
}

int h_cost(int coord){
  //parent node to goal
  return abs((coord % 10) - (goal % 10)) + abs((coord / 10) - (goal / 10));
}

int f_cost(int coord){
  
  return (g_cost(coord) + h_cost(coord));
}

void find_child_nodes(int coord){
  child_nodes[0] = coord - 1;//up

  child_nodes[1] = coord + 10;//right

  child_nodes[2] = coord + 1;//down

  child_nodes[3] = coord - 10;//left

  for(int i = 0 ; i < 4 ; i ++){
    //check if child nodes are out of bounds
    if ((child_nodes[i] < 0) || ((child_nodes[i] % 10) >= y)){
      child_nodes[i] = -1;
    }
    if ((child_nodes[i] < 0) || ((child_nodes[i] / 10) >= x)){
      child_nodes[i] = -1;
    }
    //check if the child nodes are an obstacle
    if(Coordinates[child_nodes[i] % 10][child_nodes[i] / 10][0] == 2){
      child_nodes[i] = -1;
    }
    //updates the g and h costs of the child nodes of the coordinates
    if(child_nodes[i] != -1){
      if(Coordinates[child_nodes[i] % 10][child_nodes[i] / 10][2] == 0){
        if(g_cost(child_nodes[i]) <= Coordinates[child_nodes[i] % 10][child_nodes[i] / 10][3]){
          Coordinates[child_nodes[i] % 10][child_nodes[i] / 10][3] = g_cost(child_nodes[i]);
          Coordinates[child_nodes[i] % 10][child_nodes[i] / 10][4] = h_cost(child_nodes[i]);
          Coordinates[child_nodes[i] % 10][child_nodes[i] / 10][1] = coord;
        }else if(Coordinates[child_nodes[i] % 10][child_nodes[i] / 10][3] == 0){
          Coordinates[child_nodes[i] % 10][child_nodes[i] / 10][3] = g_cost(child_nodes[i]);
          Coordinates[child_nodes[i] % 10][child_nodes[i] / 10][4] = h_cost(child_nodes[i]);
          Coordinates[child_nodes[i] % 10][child_nodes[i] / 10][1] = coord;
        }
      }
    } 
  }
}

void collect_costs(){
  //collects all the f costs and h costs of the child nodes into a list
  for(int a = 0 ; a < y ; a++ ){
    for(int b = 0 ; b < x ; b++ ){
      if((Coordinates[a][b][3] + Coordinates[a][b][4]) != 0){
        if(Coordinates[a][b][2] == 0){
          all_f_costs.add(((Coordinates[a][b][3] + Coordinates[a][b][4])*100) + (b*10) + a);//adds the f cost and the coordinates to the list
          all_h_costs.add((Coordinates[a][b][4] * 100) + (b*10) + a);//adds the h cost and the coordinates to the list
        }
      }
    }
  }
  for(int i = 0 ; i < all_f_costs.getSize() ; i++){
    if(Coordinates[(all_f_costs[i] - (int)(all_f_costs[i]/100) * 100)%10][(all_f_costs[i] - (int)(all_f_costs[i]/100) * 100)/10][2] == 1){
      all_f_costs.remove(i);
    }
  }
  for(int i = 0 ; i < all_h_costs.getSize() ; i++){
    if(Coordinates[(all_h_costs[i] - (int)(all_h_costs[i]/100) * 100)%10][(all_h_costs[i] - (int)(all_h_costs[i]/100) * 100)/10][2] == 1){
      all_h_costs.remove(i);
    }
  }
}

void createpath(){
  //updates the parent node to the current location
  parent_node = location;//updates the parent node to the current location
  find_child_nodes(parent_node);//finds the child nodes of the current location
  location = choose_smallest_value();//updates the location to the smallest h cost
  Coordinates[parent_node % 10][parent_node / 10][2] = 1;//sets the current location to visited
}

int choose_smallest_value(){
  //runs through the child nodes and finds the smallest f cost and h costs
  collect_costs();
  //puts the list values into ascending order
  ascending_order("f");
  ascending_order("h");
  //shows the number of equal, lowest f cost

  int a  = keepSmallestIdentical();

  for(int i = 0 ; i < all_h_costs.getSize() ; i++){
    for(int j = 0 ; j < a ; j++){
      if((all_h_costs[i] - (int(all_h_costs[i]/100)*100)) == (all_f_costs[j] - (int(all_f_costs[j]/100)*100))){
        return all_h_costs[i] - (all_h_costs[i]/100)*100;
        all_f_costs.remove(j);
        all_h_costs.remove(i);
        break;
      }
    }
  }  
}

int keepSmallestIdentical() {
  int a = all_f_costs.getSize();
  for(int i = 0 ; i < all_f_costs.getSize() ; i++){
    if((int(all_f_costs[0]/100)) != (int(all_f_costs[i]/100))){
      a = i;
      break;
    }
  }
  return a;
}

void ascending_order(String type){ 
  //sorts the list in ascending order
  if(type == "f"){
    int n = all_f_costs.getSize();
    int *temp_arr = all_f_costs.toArray();//converts the list to an array
    int new_temp_arr[sizeof(temp_arr)];
    int uniqueCount = 0;
    for (int i = 0; i < n - 1; i++) {
      for (int j = 0; j < n - i - 1; j++) {
        // Swap adjacent elements if they are in the wrong order
        if ((temp_arr[j]/100) > (temp_arr[j + 1]/100)) {
          int temp = temp_arr[j];//memorises the value
          temp_arr[j] = temp_arr[j + 1];//swaps the values
          temp_arr[j + 1] = temp;//swaps the values
        }
      }
    }
    removeDuplicates(temp_arr , n);
    for(int i = 0 ; i < n ; i++){
      all_f_costs.fromArray(temp_arr , n);//converts the array back to a list
      /*Serial.print("F: ");
      Serial.print(all_f_costs[i]);
      Serial.print(", ");*/
    }
    free(temp_arr); //frees the memory
  }
  else if(type == "h"){
    int n = all_h_costs.getSize();
    int *temp_arr = all_h_costs.toArray();//converts the list to an array
    int new_temp_arr[sizeof(temp_arr)];
    int uniqueCount = 0;
    for (int i = 0; i < n - 1; i++) {
      for (int j = 0; j < n - i - 1; j++) {
        // Swap adjacent elements if they are in the wrong order
        if ((temp_arr[j]/100) > (temp_arr[j + 1]/100)) {
          int temp = temp_arr[j];//memorises the value
          temp_arr[j] = temp_arr[j + 1];//swaps the values
          temp_arr[j + 1] = temp;//swaps the values
        }
      }
    }
    removeDuplicates(temp_arr , n);
    for(int i = 0 ; i < n ; i++){
      all_h_costs.fromArray(temp_arr , n);//converts the array back to a list
      /*Serial.print("H: ");
      Serial.print(all_h_costs[i]);
      Serial.print(", ");*/
    }
    free(temp_arr);//frees the memory
  }
  Serial.println("");
}

void removeDuplicates(int arr[], int &size) {
  for (int i = 0; i < size; i++) {
    for (int j = i + 1; j < size;) {
      if (arr[i] == arr[j]) {
        // Shift elements to the left to overwrite the duplicate
        for (int k = j; k < size - 1; k++) {
          arr[k] = arr[k + 1];
        }
        size--; // Reduce the size of the array
      }
      else{
        j++;
      }
    }
  }
}

void resetmap(){ //resets the map without interrupting the tile type
  for(int a = 0 ; a < y ; a++ ){
    for(int b = 0 ; b < x ; b++ ){
      for(int c = 1 ; c < 5 ; c++){
        Coordinates[a][b][c] = 0;
      }
    }
  }
  for(int a = 0 ; a < 4 ; a++ ){
    child_nodes[a] = 0;
  }
}

void finalpath(){
  int p;
  if(location != goal){
    for(int i = 0 ; i < y ; i++){
      for(int j = 0 ; j < x ; j++){
        Coordinates[i][j][0] = nGrid[i][j];
        if (nGrid[i][j] == 1) start = j*10 + i;
        if (nGrid[i][j] == 3) goal = j*10 + i;
      }
    }
    printmap();
    Coordinates[start%10][start/10][1] = start;
    Coordinates[start%10][start/10][2] = 1; //sets the current location to visited
    Coordinates[start%10][start/10][4] = h_cost(start); //sets the h cost of the current location
    createpath();
    pathmade = false;
  }
  
  else if(pathmade == false){
    path.removeAll();
    p = goal;
    while(Coordinates[p%10][p/10][0] != 1){//stay until starting point is reached
      int a = p - Coordinates[p%10][p/10][1];
      switch(a){
        case 10:
        //right
        path.addFirst('R');
        break;
        case 1:
        //down
        path.addFirst('D');
        break;
        case -10:
        //left
        path.addFirst('L');
        break;
        case -1:
        //up
        path.addFirst('U');
        break;
      }
      Coordinates[p%10][p/10][0] = 4;
      p = Coordinates[p%10][p/10][1];
      
    }
    pathmade = true;
    onetime = false;
  }
  else if (onetime == false){
    printmap();
    all_f_costs.removeAll();
    all_h_costs.removeAll();
    resetmap();
    Serial.println("");
    for(int i = 0 ; i < path.getSize() ; i++){
      Serial.print(path[i]);
      if(i<path.getSize()-1){Serial.print(", ");}
    }
    if(goal_type[0] == 'P'){
      directionMaker();
      pickUp();
    }
    else if(goal_type[0] == 'D'){
      directionMaker();
      dropOff();
    }
    else if(goal_type[0] == 'N'){
      directionMaker();
    }
    realign();
    goals.removeFirst();
    goal_type.removeFirst();
    onetime = true;
  }
  else if(goals.getSize() == placeholder.getSize()){
    Gyro('U');
    realign();
    parallelCom(1);
  }
  else if(goals.getSize() > 0){
    //reset variables
    start = current_location;
    location = current_location;
    current_destination = goals[0];
    goal = current_destination;
    nGrid[goal%10][goal/10] = 3;
    nGrid[start%10][start/10] = 1;
  }
}

void loop() { //_______________________________________________________________________________________________________________________________________________________
  finalpath();
}

void printmap() {
  Serial.println("");
  char debug_type = 'L'; // Off, Low , Med , Full , grid
  switch(debug_type){
    case 'O': //no prints
      return;
    break;
    case 'L': //prints only the tile types
      for(int a = 0 ; a < y ; a++ ){
        Serial.println("");
        for(int b = 0 ; b < x ; b++ ){
          Serial.print("|");
          if(Coordinates[a][b][0] == 4){Serial.print("◈");}
          else if(Coordinates[a][b][0] == 1){Serial.print("▣");}
          else if(Coordinates[a][b][0] == 3){Serial.print("◆");}
          else if(Coordinates[a][b][0] == 2) {Serial.print("▩");}
          //else if(Coordinates[a][b][2] == 1) {Serial.print("☑");}
          else {Serial.print("☐");}
        }
        Serial.print("|");
      }
    break;
    case 'M'://prints only required info
      Serial.println("____________________________");
      for(int a = 0 ; a < y ; a++ ){
        for(int b = 0 ; b < x ; b++ ){
          Serial.print("[");
          for(int c = 0 ; c < 1 ; c++){
            if((c == 1)||(c == 2)){ //conditions to skip info
              c++;
            }
            else{
              Serial.print(Coordinates[a][b][c]);
              Serial.print(" , ");
            }
          }
        Serial.print("]");
        Serial.print(" , ");
      }
      Serial.println("");
      }
      Serial.print("____________________________");
    break;
    case 'G'://prints parallel COM - ngrid
    Serial.println("____________________________");
      for(int a = 0 ; a < y ; a++ ){
        for(int b = 0 ; b < x ; b++ ){
          Serial.print("[");
          Serial.print(nGrid[a][b]);
          Serial.print("]");
          Serial.print(" , ");
        }
        Serial.println("");
      }
      Serial.print("____________________________");
    break;
    case 'F'://prints everything
      Serial.println("____________________________");
      for(int a = 0 ; a < y ; a++ ){
        for(int b = 0 ; b < x ; b++ ){
          Serial.print("[");
          for(int c = 0 ; c < 5 ; c++){
            Serial.print(Coordinates[a][b][c]);
            Serial.print(" , ");
          }
        Serial.print("]");
        Serial.print(" , ");
      }
      Serial.println("");
      }
      Serial.print("____________________________");
    break;
  } 
}