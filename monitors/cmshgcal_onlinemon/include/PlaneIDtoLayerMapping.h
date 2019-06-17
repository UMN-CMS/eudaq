int PlaneIDtoLayerMapping(int id_plane){

  int ilayer = 0;
  if(id_plane<30) return id_plane+1; //EE
  ilayer = 29 + (int)(id_plane-30/7);

  if(id_plane>94) return ilayer;
  
  return ilayer;

}
