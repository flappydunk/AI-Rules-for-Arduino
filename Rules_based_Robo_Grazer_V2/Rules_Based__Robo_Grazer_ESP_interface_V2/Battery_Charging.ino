
void Charge_monitor(){    
  //check charge threshold
   if ((bitRead(system_current,8) == 1) && (Sender_Current < batFullCurrent )){
      bitSet(system_current,9);
   }
   else{
      bitClear(system_current,9);
   }
}
 
