#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <vector>
#include <cassert>

#include "MersenneTwister.h"

#include "tile.h"
#include "location.h"
#include "board.h"


// this global variable is set in main.cpp and is adjustable from the command line
// (you are not allowed to make your own global variables)
int GLOBAL_TILE_SIZE = 11;


// ==========================================================================
// Helper function that is called when an error in the command line
// arguments is detected.
void usage(int argc, char *argv[]) {
  std::cerr << "USAGE: " << std::endl;
  std::cerr << "  " << argv[0] << " <filename>  -board_dimensions <h> <w>" << std::endl;
  std::cerr << "  " << argv[0] << " <filename>  -board_dimensions <h> <w>  -all_solutions" << std::endl;
  std::cerr << "  " << argv[0] << " <filename>  -board_dimensions <h> <w>  -allow_rotations" << std::endl;
  std::cerr << "  " << argv[0] << " <filename>  -all_solutions  -allow_rotations" << std::endl;
  std::cerr << "  " << argv[0] << " <filename>  -tile_size <odd # >= 11>" << std::endl;
  exit(1);
}



// ==========================================================================
// To get you started, this function places tiles on the board and
// randomly and outputs the results (in all likelihood *not* a
// solution!) in the required format
void RandomlyPlaceTiles(Board &board, const std::vector<Tile*> &tiles, std::vector<Location> &locations) {

  // MersenneTwister is an excellent library for psuedo-random numbers!
  MTRand mtrand;

  for (int t = 0; t < tiles.size(); t++) {
    // loop generates random locations until we (eventually) find one
    // that is not occupied
    int i,j;
    do {
      // generate a random coordinate within the range 0,0 -> rows-1,cols-1
      i = mtrand.randInt(board.numRows()-1);
      j = mtrand.randInt(board.numColumns()-1);
    } while (board.getTile(i,j) != NULL);

    // rotation is always 0 (for now)
    locations.push_back(Location(i,j,0));
    board.setTile(i,j,tiles[t]);
  }
}


// ==========================================================================
void HandleCommandLineArguments(int argc, char *argv[], std::string &filename, 
                                int &rows, int &columns, bool &all_solutions, bool &allow_rotations) {

  // must at least put the filename on the command line
  if (argc < 2) {
    usage(argc,argv);
  }
  filename = argv[1];

  // parse the optional arguments
  for (int i = 2; i < argc; i++) {
    if (argv[i] == std::string("-tile_size")) {
      i++;
      assert (i < argc);
      GLOBAL_TILE_SIZE = atoi(argv[i]);
      if (GLOBAL_TILE_SIZE < 11 || GLOBAL_TILE_SIZE % 2 == 0) {
        std::cerr << "ERROR: bad tile_size" << std::endl;
        usage(argc,argv);
      }
    } else if (argv[i] == std::string("-all_solutions")) {
      all_solutions = true;
    } else if (argv[i] == std::string("-board_dimensions")) {
      i++;
      assert (i < argc);
      rows = atoi(argv[i]);
      i++;
      assert (i < argc);
      columns = atoi(argv[i]);
      if (rows < 1 || columns < 1) {
        usage(argc,argv);
      }
    } else if (argv[i] == std::string("-allow_rotations")) {
      allow_rotations = true;
    } else {
      std::cerr << "ERROR: unknown argument '" << argv[i] << "'" << std::endl;
      usage(argc,argv);
    }
  }
}


// ==========================================================================
void ParseInputFile(int argc, char *argv[], const std::string &filename, std::vector<Tile*> &tiles) {

  // open the file
  std::ifstream istr(filename.c_str());
  if (!istr) {
    std::cerr << "ERROR: cannot open file '" << filename << "'" << std::endl;
    usage(argc,argv);
  }
  assert (istr);

  // read each line of the file
  std::string token, north, east, south, west;
  while (istr >> token >> north >> east >> south >> west) {
    assert (token == "tile");
    Tile *t = new Tile(north,east,south,west);
    tiles.push_back(t);
  }
}

//============================================================================
bool NotInVector(const std::vector<Location>& Nearby, int r, int c){
  for(int i=0; i<Nearby.size(); i++){
    if (r==Nearby[i].row && c==Nearby[i].column)
      return false;
  }
  return true;
}

//===========================================================================
void FindNearby(Board& board, std::vector<Location>& Nearby, const std::vector<Location>& locations){
  
  bool Empty=true;
  Nearby.clear();
  for (int i=0; i<board.numRows(); i++){
    for (int j=0; j<board.numColumns(); j++){
      if(board.getTile(i, j)!=NULL){
        Empty=false;
        //std::cout << "FindNearby" <<std::endl;
        if (i+1>=0 && i+1 <board.numRows()){
          if(NotInVector(Nearby, i+1, j) && NotInVector(locations, i+1, j) )
            Nearby.push_back(Location(i+1,j,0));
        }
        if (i-1>=0 && i-1 <board.numRows()){
          if(NotInVector(Nearby, i-1, j) && NotInVector(locations, i-1, j) )
            Nearby.push_back(Location(i-1,j,0));
        }
        if (j+1>=0 && j+1 <board.numColumns()){
          if(NotInVector(Nearby, i, j+1) && NotInVector(locations, i, j+1) )
            Nearby.push_back(Location(i,j+1,0));
        }
        if (j-1>=0 && j-1 <board.numColumns()){
          if(NotInVector(Nearby, i, j-1) && NotInVector(locations, i, j-1) )
            Nearby.push_back(Location(i,j-1,0));
        }
        
      }
    }
  }
  if (Empty==true){
    Nearby.push_back(Location(4, 4, 0));
  }
}
//===========================================================================
bool NotLoose(Board& board, std::vector<Location>& locations){
  //std::cout<< "here5 " << std::endl;
  for (int i; i<locations.size(); i++){
          //std::cout<< "here5 " << std::endl;
    Tile temp=board[locations[i].row][locations[i].column]->rotate(board[locations[i].row][locations[i].column]->getRotation());
    if (temp.getNorth()!="pasture"){
      if(locations[i].row-1<0)
        return false;
      else if(board[locations[i].row-1][locations[i].column]==NULL)
        return false;
    }
    if (temp.getSouth()!="pasture"){

      if(locations[i].row+1>board.numRows())
        return false;
      else if(board[locations[i].row+1][locations[i].column]==NULL)
        return false;
    }
    if(temp.getWest()!="pasture"){
      if(locations[i].column-1<0)
        return false;
      else if(board[locations[i].row][locations[i].column-1]==NULL)
        return false;
    }
    if(temp.getEast()!="pasture"){
      if(locations[i].column+1>board.numColumns())
        return false;
      else if(board[locations[i].row][locations[i].column+1]==NULL)
        return false;
    }
  }
  std::cout<< "here5 " << std::endl;
  return true;
}
//============================================================================
bool match(Board& board, int r, int c, std::vector<Tile*>& tiles, std::vector<Location>& locations){
  std::cout << "test match of " << locations.size()-1 << " tile in location ("<< r <<","<< c <<"). rotation "<<locations.back().rotation << std::endl;
  //Location MatchPiece(r, c, 0);
  bool north=true;
  bool south=true;
  bool west=true;
  bool east=true;
  int level=0;
  int n=locations.size()-1;
  Tile temp=tiles[n]->rotate(locations.back().rotation);
  std::cout << temp.getNorth() << " " << temp.getEast() <<" "<< temp.getSouth() << " "<< temp.getWest()<< std::endl;
  if (r==0 || r==board.numRows()-1 || c==0 || c==board.numColumns()-1 ){
    if (r==0 && c==0 ){
      if (board.getTile(r+1, c)!=NULL){
        if (temp.getSouth() != board.getTile(r+1, c)->rotate(board.getTile(r+1, c)->getRotation()).getNorth())
          south=false;
        else if (temp.getSouth()!="pasture")
          level++;
      }
      if (board.getTile(r, c+1)!=NULL){
        if (temp.getEast() != board.getTile(r, c+1)->rotate(board.getTile(r, c+1)->getRotation()).getWest())
          east=false;
        else if (temp.getEast()!="pasture")
          level++;
      }
    }
    if (r==0 && c==board.numColumns()-1 ){
      if (board.getTile(r+1, c)!=NULL){
        if (temp.getSouth() != board.getTile(r+1, c)->rotate(board.getTile(r+1, c)->getRotation()).getNorth())
          south=false;
        else if (temp.getSouth()!="pasture")
          level++;
      }
      if (board.getTile(r, c-1)!=NULL){
        if (temp.getWest() != board.getTile(r, c-1)->rotate(board.getTile(r, c-1)->getRotation()).getEast())
          west=false;
        else if (temp.getWest()!="pasture")
          level++;
      }
    }

    if (r==board.numRows()-1 && c==board.numColumns()-1 ){
      if (board.getTile(r-1, c)!=NULL){
        if (temp.getNorth() != board.getTile(r-1, c)->rotate(board.getTile(r-1, c)->getRotation()).getSouth())
          north=false;
        else if (temp.getNorth()!="pasture")
          level++;
      }
      if (board.getTile(r, c-1)!=NULL){
        if (temp.getWest() != board.getTile(r, c-1)->rotate(board.getTile(r, c-1)->getRotation()).getEast())
          west=false;
        else if (temp.getWest()!="pasture")
          level++;
      }
    }
    if (r==board.numRows()-1 && c==0){
      if (board.getTile(r-1, c)!=NULL){
        if (temp.getNorth() != board.getTile(r-1, c)->rotate(board.getTile(r-1, c)->getRotation()).getSouth())
          north=false;
        else if (temp.getNorth()!="pasture")
          level++;
      }
      if (board.getTile(r, c+1)!=NULL){
        if (temp.getEast() != board.getTile(r, c+1)->rotate(board.getTile(r, c+1)->getRotation()).getWest())
          east=false;
        else if (temp.getEast()!="pasture")
          level++;
      }
    }
    if (r==0 && c!=0 && c!=board.numColumns()-1 ){
      if (board.getTile(r+1, c)!=NULL){
        if (temp.getSouth() != board.getTile(r+1, c)->rotate(board.getTile(r+1, c)->getRotation()).getNorth())
          south=false;
        else if (temp.getSouth()!="pasture")
          level++;
      }
      if (board.getTile(r, c+1)!=NULL){
        if (temp.getEast() != board.getTile(r, c+1)->rotate(board.getTile(r, c+1)->getRotation()).getWest())
          east=false;
        else if (temp.getEast()!="pasture")
          level++;
      }
      if (board.getTile(r, c-1)!=NULL){
        if (temp.getWest() != board.getTile(r, c-1)->rotate(board.getTile(r, c-1)->getRotation()).getEast())
          west=false;
        else if (temp.getWest()!="pasture")
          level++;
      }
    }
    if (r!=0 && r!=board.numRows()-1 && c==board.numColumns()-1 ){
      //std::cout << "heer"<< std::endl;
      if (board.getTile(r-1, c)!=NULL){
        if (temp.getNorth() != board.getTile(r-1, c)->rotate(board.getTile(r-1, c)->getRotation()).getSouth())
          north=false;
        else if (temp.getNorth()!="pasture")
          level++;
      }
      if (board.getTile(r+1, c)!=NULL){
        if (temp.getSouth() != board.getTile(r+1, c)->rotate(board.getTile(r+1, c)->getRotation()).getNorth())
          south=false;
        else if (temp.getSouth()!="pasture")
          level++;
      }
      if (board.getTile(r, c-1)!=NULL){
        if (temp.getWest() != board.getTile(r, c-1)->rotate(board.getTile(r, c-1)->getRotation()).getEast()){
          //std::cout<< "here"<< r << c-1<< temp.getWest()<<board.getTile(r, c-1)->rotate(board.getTile(r, c-1)->getRotation()).getEast()<< board.getTile(r, c-1)->rotate(board.getTile(r, c-1)->getRotation()).getWest()
         //<<board.getTile(r, c-1)->rotate(board.getTile(r, c-1)->getRotation()).getNorth()<<board.getTile(r, c-1)->rotate(board.getTile(r, c-1)->getRotation()).getSouth()<<std::endl;
          west=false;
        }
        else if (temp.getWest()!="pasture")
          level++;
      }
      //std::cout << North << west << std::endl;
    }
    if (r==board.numRows()-1 && c!=0 && c!=board.numColumns()-1 ){
      //std::cout << "here" << std::endl;
      if (board.getTile(r-1, c)!=NULL){
        if (temp.getNorth() != board.getTile(r-1, c)->rotate(board.getTile(r-1, c)->getRotation()).getSouth())
          north=false;
        else if (temp.getNorth()!="pasture")
          level++;
      }
      if (board.getTile(r, c+1)!=NULL){
        if (temp.getEast() != board.getTile(r, c+1)->rotate(board.getTile(r, c+1)->getRotation()).getWest())
          east=false;
        else if (temp.getEast()!="pasture")
          level++;
      }
      if (board.getTile(r, c-1)!=NULL){
        if (temp.getWest() != board.getTile(r, c-1)->rotate(board.getTile(r, c-1)->getRotation()).getEast())
          west=false;
        else if (temp.getWest()!="pasture")
          level++;
      }
      //std::cout << "here" << std::endl;
    }
    if (r!=0 && r!=board.numRows()-1 && c==0){
      if (board.getTile(r-1, c)!=NULL){
        if (temp.getNorth() != board.getTile(r-1, c)->rotate(board.getTile(r-1, c)->getRotation()).getSouth())
          north=false;
        else if (temp.getNorth()!="pasture")
          level++;
      }
      if (board.getTile(r+1, c)!=NULL){
        if (temp.getSouth() != board.getTile(r+1, c)->rotate(board.getTile(r+1, c)->getRotation()).getNorth())
          south=false;
        else if (temp.getSouth()!="pasture")
          level++;
      }
      if (board.getTile(r, c+1)!=NULL){
        if (temp.getEast() != board.getTile(r, c+1)->rotate(board.getTile(r, c+1)->getRotation()).getWest())
          east=false;
        else if (temp.getEast()!="pasture")
          level++;
      }
    }
  }
  //std::cout << "test match2" <<std::endl;
  else {
  if (board.getTile(r-1, c)!=NULL){
    if (temp.getNorth() != board.getTile(r-1, c)->rotate(board.getTile(r-1, c)->getRotation()).getSouth())
      north=false;
    else if (temp.getNorth()!="pasture")
      level++;
  }
  if (board.getTile(r+1, c)!=NULL){
    if (temp.getSouth() != board.getTile(r+1, c)->rotate(board.getTile(r+1, c)->getRotation()).getNorth())
      south=false;
    else if (temp.getSouth()!="pasture")
      level++;
  }
  if (board.getTile(r, c+1)!=NULL){
    if (temp.getEast() != board.getTile(r, c+1)->rotate(board.getTile(r, c+1)->getRotation()).getWest())
      east=false;
    else if (temp.getEast()!="pasture")
      level++;
  }
  if (board.getTile(r, c-1)!=NULL){
    if (temp.getWest() != board.getTile(r, c-1)->rotate(board.getTile(r, c-1)->getRotation()).getEast())
      west=false;
    else if (temp.getWest()!="pasture")
      level++;
  }
  }
  //std::cout << "heer"<< std::endl;
  /*
  if ((temp.getNorth() != board.getTile(r-1, c)->rotate(board.getTile(r-1, c)->getRotation()).getNorth()||board.getTile(r-1, c)==NULL) 
      && (temp.getSouth() == board.getTile(r+1,c)->getSouth()||board.getTile(r+1, c)==NULL)
      && (temp.getEast() == board.getTile(r,c+1)->getEast()||board.getTile(r, c+1)==NULL)
      && (temp.getWest() == board.getTile(r,c-1)->getWest()||board.getTile(r, c-1)==NULL)){
    std::cout << "did not find a solution" <<std::endl;
    return true;
  }
  */
  if(north ==false || south==false || east==false || west==false){
    std::cout << north << " " <<south << " "<< east << " " << west<< "not matching"<<std::endl;
    return false;
  }
  int NumNULL=0;
  int NumTiles=4;
  if (r+1 <board.numRows()){
    if (board.getTile(r+1,c)==NULL)
      NumNULL++;
  }
  else
    NumTiles--;
  if (r-1>=0){
    if (board.getTile(r-1,c)==NULL)
      NumNULL++;
  }
  else
    NumTiles--;
  if (c+1 < board.numColumns()){
    if (board.getTile(r,c+1)==NULL)
      NumNULL++;
  }
  else
    NumTiles--;
  if (c-1 >= 0) {
    if( board.getTile(r,c-1)==NULL)
      NumNULL++;
  }
  else
    NumTiles--;
  //std::cout << "level: "<<level<<"NumTiles: "<<NumTiles<<"NumNULL: "<<NumNULL <<std::endl;
  if (level == NumTiles-NumNULL){
    //std::cout<< "size of location " << locations.size()<<std::endl;
    if (locations.size()==tiles.size()){
      board.setTile(r,c,tiles[n]);
      if ( NotLoose(board, locations) ){
        //std::cout<< "size of location" << locations.size()<<std::endl;
        board.setTile(r,c,NULL);
        return true;
      }
      else
        board.setTile(r,c,NULL);
        return false;
    }
    else
      return true;
  }
  else
    return false;
}

//===========================================================================
bool FindSolution(Board& board, std::vector<Tile*>& tiles, std::vector<Location>& locations){
  if (locations.size()==tiles.size()){
    //std::cout << "tested1" << std::endl;
    return true;
  }
  std::vector<Location> Nearby;
  FindNearby(board, Nearby, locations);
  for(int j=0; j<Nearby.size(); j++){//output the nearby positions.
    std::cout << "( "<<Nearby[j].row<<" , "<<Nearby[j].column<<" )"<<std::endl;
  }
  for(int i=0; i<Nearby.size(); i++){
    std::cout << "finding a match at ("<<Nearby[i].row << ","<<Nearby[i].column<<"), for the "<<locations.size()<<" tile."<<std::endl;
    locations.push_back(Location(Nearby[i].row, Nearby[i].column, 0));
    if (match(board, Nearby[i].row, Nearby[i].column, tiles, locations) ){
      std::cout << "found a match at ("<<Nearby[i].row << ","<<Nearby[i].column<<"), for the "<<locations.size()-1<<" tile."<<std::endl;
      board.setTile(Nearby[i].row, Nearby[i].column, tiles[locations.size()-1]);//the next tile
      if(FindSolution(board, tiles, locations))
        return true;
      else{
        board.setTile(Nearby[i].row, Nearby[i].column, NULL);
        locations.pop_back();
      }
    }
    else
      locations.pop_back();
  }
  return false;
}
//==============================================================================
bool FindSolutionR(Board& board, std::vector<Tile*>& tiles, std::vector<Location>& locations){
  if (locations.size()==tiles.size()){
    //std::cout << "tested1" << std::endl;
    return true;
  }
  std::vector<Location> Nearby;
  FindNearby(board, Nearby, locations);
  for(int j=0; j<Nearby.size(); j++){//output the nearby positions.
    std::cout << "( "<<Nearby[j].row<<" , "<<Nearby[j].column<<" )"<<std::endl;
  }
  for(int i=0; i<Nearby.size(); i++){
    //std::cout << "finding a match at ("<<Nearby[i].row << ","<<Nearby[i].column<<"), for the "<<locations.size()<<" tile."<<std::endl;
    locations.push_back(Location(Nearby[i].row, Nearby[i].column, 0));
    if (match(board, Nearby[i].row, Nearby[i].column, tiles, locations) ){
      //std::cout << "found a match at ("<<Nearby[i].row << ","<<Nearby[i].column<<"), for the "<<locations.size()-1<<" tile."<<std::endl;
      board.setTile(Nearby[i].row, Nearby[i].column, tiles[locations.size()-1]);//the next tile
      if(FindSolutionR(board, tiles, locations))
        return true;
      else{
        tiles[locations.size()-1]->rotate(0);
        board.setTile(Nearby[i].row, Nearby[i].column, NULL);
        locations.pop_back();
      }
    }
    else{
      tiles[locations.size()-1]->rotate(0);
      locations.pop_back();
    }

    //rotate by 90
    locations.push_back(Location(Nearby[i].row, Nearby[i].column, 90));
    if (match(board, Nearby[i].row, Nearby[i].column, tiles, locations) ){
      //std::cout << "found a match at ("<<Nearby[i].row << ","<<Nearby[i].column<<"), for the "<<locations.size()-1<<" tile. rotation 90"<<std::endl;
      board.setTile(Nearby[i].row, Nearby[i].column, tiles[locations.size()-1]);//the next tile
      if(FindSolutionR(board, tiles, locations))
        return true;
      else{
        tiles[locations.size()-1]->rotate(0);
        board.setTile(Nearby[i].row, Nearby[i].column, NULL);
        locations.pop_back();
      }
    }
    else{
      tiles[locations.size()-1]->rotate(0);
      locations.pop_back();
    }

    locations.push_back(Location(Nearby[i].row, Nearby[i].column, 180));
    if (match(board, Nearby[i].row, Nearby[i].column, tiles, locations) ){
      //std::cout << "found a match at ("<<Nearby[i].row << ","<<Nearby[i].column<<"), for the "<<locations.size()-1<<" tile.rotation 180"<<std::endl;
      board.setTile(Nearby[i].row, Nearby[i].column, tiles[locations.size()-1]);//the next tile
      if(FindSolutionR(board, tiles, locations))
        return true;
      else{
        tiles[locations.size()-1]->rotate(0);
        board.setTile(Nearby[i].row, Nearby[i].column, NULL);
        locations.pop_back();
      }
    }
    else{
      tiles[locations.size()-1]->rotate(0);
      locations.pop_back();
    }

    locations.push_back(Location(Nearby[i].row, Nearby[i].column, 270));
    if (match(board, Nearby[i].row, Nearby[i].column, tiles, locations) ){
      //std::cout << "found a match at ("<<Nearby[i].row << ","<<Nearby[i].column<<"), for the "<<locations.size()-1<<" tile. rotation 270"<<std::endl;
      board.setTile(Nearby[i].row, Nearby[i].column, tiles[locations.size()-1]);//the next tile
      if(FindSolutionR(board, tiles, locations))
        return true;
      else{
        tiles[locations.size()-1]->rotate(0);
        board.setTile(Nearby[i].row, Nearby[i].column, NULL);
        locations.pop_back();
      }
    }
    else{
      tiles[locations.size()-1]->rotate(0);
      locations.pop_back();
    }
  }
  return false;
}
//==========================================================================
void FindAllSolutionsR(Board& board, std::vector<Tile*>& tiles, std::vector<Location>& locations, std::vector<std::vector<Location> >& solutions){

  std::vector<Location> Nearby;
  FindNearby(board, Nearby, locations);
  for(int j=0; j<Nearby.size(); j++){//output the nearby positions.
    std::cout << "( "<<Nearby[j].row<<" , "<<Nearby[j].column<<" )"<<std::endl;
  }
  for(int i=0; i<Nearby.size(); i++){
    std::cout << "finding a match at ("<<Nearby[i].row << ","<<Nearby[i].column<<"), for the "<<locations.size()<<" tile."<<std::endl;
    locations.push_back(Location(Nearby[i].row, Nearby[i].column, 0));
    if (match(board, Nearby[i].row, Nearby[i].column, tiles, locations) ){
      std::cout << "found a match at ("<<Nearby[i].row << ","<<Nearby[i].column<<"), for the "<<locations.size()-1<<" tile."<<std::endl;
      board.setTile(Nearby[i].row, Nearby[i].column, tiles[locations.size()-1]);//the next tile
      if (!FindSolutionR(board, tiles, locations)){
        locations.pop_back();
        board.setTile(Nearby[i].row, Nearby[i].column, NULL);
      }
      else{
      if(locations.size()==tiles.size()){
        solutions.push_back(locations);
        locations.pop_back();
      }
      else{
        FindAllSolutionsR(board, tiles, locations, solutions);
      }
    }
    }
    else
      locations.pop_back();

    //rotation by 90
    locations.push_back(Location(Nearby[i].row, Nearby[i].column, 90));
    if (match(board, Nearby[i].row, Nearby[i].column, tiles, locations) ){
      std::cout << "found a match at ("<<Nearby[i].row << ","<<Nearby[i].column<<"), for the "<<locations.size()-1<<" tile."<<std::endl;
      board.setTile(Nearby[i].row, Nearby[i].column, tiles[locations.size()-1]);//the next tile
      if (!FindSolutionR(board, tiles, locations)){
        locations.pop_back();
        board.setTile(Nearby[i].row, Nearby[i].column, NULL);
      }
      else{
      if(locations.size()==tiles.size()){
        solutions.push_back(locations);
        locations.pop_back();
      }
      else{
        FindAllSolutionsR(board, tiles, locations, solutions);
      }
    }
    }
    else
      locations.pop_back();

    //rotation by 180
    locations.push_back(Location(Nearby[i].row, Nearby[i].column, 180));
    if (match(board, Nearby[i].row, Nearby[i].column, tiles, locations) ){
      std::cout << "found a match at ("<<Nearby[i].row << ","<<Nearby[i].column<<"), for the "<<locations.size()-1<<" tile."<<std::endl;
      board.setTile(Nearby[i].row, Nearby[i].column, tiles[locations.size()-1]);//the next tile
      if (!FindSolutionR(board, tiles, locations)){
        locations.pop_back();
        board.setTile(Nearby[i].row, Nearby[i].column, NULL);
      }
      else{
      if(locations.size()==tiles.size()){
        solutions.push_back(locations);
        locations.pop_back();
      }
      else{
        FindAllSolutionsR(board, tiles, locations, solutions);
      }
    }
    }
    else
      locations.pop_back();

    //rotation by 270
    locations.push_back(Location(Nearby[i].row, Nearby[i].column, 270));
    if (match(board, Nearby[i].row, Nearby[i].column, tiles, locations) ){
      std::cout << "found a match at ("<<Nearby[i].row << ","<<Nearby[i].column<<"), for the "<<locations.size()-1<<" tile."<<std::endl;
      board.setTile(Nearby[i].row, Nearby[i].column, tiles[locations.size()-1]);//the next tile
      if (!FindSolutionR(board, tiles, locations)){
        locations.pop_back();
        board.setTile(Nearby[i].row, Nearby[i].column, NULL);
      }
      else{
      if(locations.size()==tiles.size()){
        solutions.push_back(locations);
        locations.pop_back();
      }
      else{
        FindAllSolutionsR(board, tiles, locations, solutions);
      }
    }
    }
    else
      locations.pop_back();

  }
  //return false;
}
//==========================================================================
void FindAllSolutions(Board& board, std::vector<Tile*>& tiles, std::vector<Location>& locations, std::vector<std::vector<Location> >& solutions){

  std::vector<Location> Nearby;
  FindNearby(board, Nearby, locations);
  for(int j=0; j<Nearby.size(); j++){//output the nearby positions.
    std::cout << "( "<<Nearby[j].row<<" , "<<Nearby[j].column<<" )"<<std::endl;
  }
  for(int i=0; i<Nearby.size(); i++){
    std::cout << "finding a match at ("<<Nearby[i].row << ","<<Nearby[i].column<<"), for the "<<locations.size()<<" tile."<<std::endl;
    locations.push_back(Location(Nearby[i].row, Nearby[i].column, 0));
    if (match(board, Nearby[i].row, Nearby[i].column, tiles, locations) ){
      std::cout << "found a match at ("<<Nearby[i].row << ","<<Nearby[i].column<<"), for the "<<locations.size()-1<<" tile."<<std::endl;
      board.setTile(Nearby[i].row, Nearby[i].column, tiles[locations.size()-1]);//the next tile
      if (!FindSolution(board, tiles, locations)){
        locations.pop_back();
        board.setTile(Nearby[i].row, Nearby[i].column, NULL);
      }
      else{
      if(locations.size()==tiles.size()){
        solutions.push_back(locations);
        locations.pop_back();
      }
      else{
        FindAllSolutions(board, tiles, locations, solutions);
      }
    }
    }
    else
      locations.pop_back();
  }
  //return false;
}
// ==========================================================================
int main(int argc, char *argv[]) {

  std::string filename;
  int rows = -1;
  int columns = -1;
  bool all_solutions = false;
  bool allow_rotations = false;
  HandleCommandLineArguments(argc, argv, filename, rows, columns, all_solutions, allow_rotations);


  // load in the tiles
  std::vector<Tile*> tiles;
  ParseInputFile(argc,argv,filename,tiles);


  // confirm the specified board is large enough
  if (rows < 1  ||  columns < 1  ||  rows * columns < tiles.size()) {
    std::cerr << "ERROR: specified board is not large enough" << rows << "X" << columns << "=" << rows*columns << " " << tiles.size() << std::endl;
    usage(argc,argv);
  }

  /*
  for (int i = 0; i < 5; i++) {

    // generate a random tile layouts
    Board board(rows,columns);
    std::vector<Location> locations;
    RandomlyPlaceTiles(board, tiles, locations);
    
    // print the solution
    std::cout << "probably-not-a-Solution: ";
    for (int i = 0; i < locations.size(); i++) {
      std::cout << locations[i];
    }
    std::cout << std::endl;

    // print the ASCII art board representation
    board.Print();
    std::cout << std::endl;
  }
  */
  Board board(rows,columns);
  std::vector<Location> locations;
  std::vector<std::vector<Location> > solutions;
  //std::vector<Location> Nearby;
if (all_solutions==true && allow_rotations==true){
  //locations.clear();

  FindAllSolutionsR(board, tiles, locations, solutions);
  if(solutions.size()==0){
    std::cout << "did not find a solution" <<std::endl;
  }
  else{
    std::cout << "found "<<solutions.size()<<" solutions."<<std::endl;
    for (int i = 0; i < solutions.size(); i++) {
      std::cout << "This is a solution: ";
      for(int j=0; j< tiles.size(); j++){
        std::cout << solutions[i][j];
      }
      std::cout << std::endl;
    }

  }
}

if (all_solutions==false && allow_rotations==true){
  if (FindSolutionR(board, tiles, locations)) {

    // generate a random tile layouts

    //RandomlyPlaceTiles(board, tiles, locations);
    
    // print the solution
    std::cout << "This is a solution: ";
    for (int i = 0; i < locations.size(); i++) {
      std::cout << locations[i];
    }
    std::cout << std::endl;

    // print the ASCII art board representation
    board.Print();
    std::cout << std::endl;
  }
  else
    std::cout << "did not find a solution" <<std::endl;

  // delete the tiles

}

if (all_solutions==false && allow_rotations==false){
  if (FindSolution(board, tiles, locations)) {

    // generate a random tile layouts

    //RandomlyPlaceTiles(board, tiles, locations);
    
    // print the solution
    std::cout << "This is a solution: ";
    for (int i = 0; i < locations.size(); i++) {
      std::cout << locations[i];
    }
    std::cout << std::endl;

    // print the ASCII art board representation
    board.Print();
    std::cout << std::endl;
  }
  else
    std::cout << "did not find a solution" <<std::endl;

  // delete the tiles

}
if (all_solutions==true && allow_rotations==false){
  //locations.clear();

  FindAllSolutions(board, tiles, locations, solutions);
  if(solutions.size()==0){
    std::cout << "did not find a solution" <<std::endl;
  }
  else{
    std::cout << "found "<<solutions.size()<<" solutions."<<std::endl;
    for (int i = 0; i < solutions.size(); i++) {
      std::cout << "This is a solution: ";
      for(int j=0; j< tiles.size(); j++){
        std::cout << solutions[i][j];
      }
      std::cout << std::endl;
    }

  }
}

  for (int t = 0; t < tiles.size(); t++) {
    delete tiles[t];
  }
}
// ==========================================================================
