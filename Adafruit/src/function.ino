void deconnexion() {
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("\r\nDeconnexion"));
  }
  AT_pass = false;

  //Deconnexion serveur + reseau GPRS : LTE-M / NB-IOT / 5G
  if ((networkConnection == true) and (((statePSM == false) and (stateEDRX == false)) or (extinction_force_ublox == true))) {// and (desactivatePsmFailed == false)) {
    /*unsigned long AT_currentMillis = millis();
    unsigned long AT_previousMillis = millis();
    while ((AT_pass == false) and (AT_currentMillis - AT_previousMillis <= 900)) { //5000
      ATenvoi("AT");
      if (ATecoute("AT", "AT", 1000) == 1) {
        AT_pass = true;
      }
      AT_currentMillis = millis();
    }*/

    ATenvoi("AT");
    if (ATecoute("AT", "AT", 1000) == 1) {
      AT_pass = true;
    }

    sodaq_wdt_safe_delay(100); //tempo de secu
    clignotement_led_easypush_temp_easycompact("cligno");

    //Deco serveur MQTT
    if ((serverConnection == true) and (AT_pass == true)) { 
      ATenvoi("AT+UMQTTC=0"); //CFUN=4
      if (ATecoute("AT+UMQTTC=0", "+UMQTTC: 0,", 5000) == 1) { 
        AT_pass = true; //Pour le if en dessous
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("Deconnexion serveur OK"));
        }
      }
      serverConnection = false;
    }
    else {
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("Deco non utile serveur !"));
      }
    }

    //Déco réseau
    if (AT_pass == true){
      sodaq_wdt_safe_delay(100);//Tempo obligatoire sinon erreur sur AT+CGATT
      ATenvoi("AT+CGATT=0"); //CFUN=4
      if (ATecoute("AT+CGATT=0", "AT+CGATT=0", 10000) == 1) { //10 sec, car le OK peut venir assez loin
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("Deconnecte"));
        }
        networkConnection = false;
      }
    }
    //Extinction des leds
    if (activateLED == true) {
      gestion_led("eteint");
    }
  }
  else {
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("Deconnexion non utile reseau !"));
      DEBUG_STREAM.println("networkConnection : "+String(networkConnection));
      DEBUG_STREAM.println("statePSM : "+String(statePSM));
      DEBUG_STREAM.println("stateEDRX : "+String(stateEDRX));
      DEBUG_STREAM.println("desactivatePsmFailed : "+String(desactivatePsmFailed));
      DEBUG_STREAM.println("desactivateEdrxFailed : "+String(desactivateEdrxFailed));
    }
  }

  //Extinction des leds
  if (activateLED == true) {
    gestion_led("eteint");
  }

  //Mise a false des variables
  networkConnection = false;
  serverConnection = false;
}

void deco_serveur_mqtt() {
  ATenvoi("AT+UMQTTC=0"); //CFUN=4
  if (ATecoute("AT+UMQTTC=0", "+UMQTTC: 0,", 5000) == 1) { 
    serverConnection = false;
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("Deco serveur OK"));
    }
  }
  else {
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("Echec deco serveur, OFF ALL !"));
    }
    power_off_saraR4();
    networkConnection = false;
    serverConnection = false;
  }
}


void deep_sleep_sara()
{
  //Sleep module Sara
  
}

void reboot_sodaq()
{
  //Reboot carte Sodaq SFF
  
}

void verification_ok(String at_c1)
{
  if (((at_c1.indexOf("/9/") > 0) and (at_c1.indexOf("+UHTTPC=0,5,") > 0)) or (at_c1.indexOf("+URDFILE=") > 0) or (at_c1.indexOf("+UDNSRN=0") > 0)) { //pour éviter le reboot WDT à cause du probleme sécu Paprec
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("Demarrage verif position OK"));
    }
  }

  //On vérifie dans cette fonction que le OK est bien en fin de commande
  int PositionOfAT = reponse.indexOf(at_c1);
  String avant_at = reponse.substring(0,PositionOfAT);
  String apres_at = reponse.substring(PositionOfAT,reponse.length());
  
  if (avant_at.indexOf("OK") > 0) { 
    //On supprime le OK de la reponse 
    reponse =  "\n" + apres_at;
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("Reponse MAJ sans OK :")); 
      DEBUG_STREAM.println(String(reponse)); 
    }
  }

  if (apres_at.indexOf("OK") > 0) {
    position_du_ok = true;
  }
  else {
    //Il nous manque le OK en fin
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("OK de fin de la commande AT absent")); 
    }
    position_du_ok = false;
  }
}



void construction_trame(String mode_construction) {
  if (DEBUG_SERIAL == true) { 
    DEBUG_STREAM.println("\r\nStart build trame type_trame : "+String(type_trame));
  }
  //Clear Json
  trame_json.clear();
  //Obtention voltage batterie
  getBatteryVoltage();
  //Convert duree_precedente_envoi_trame
  unsigned long duree_precedente_envoi_trame_sec = 0;
  if (mode_construction == "basique") {
    if (duree_precedente_envoi_trame == 0) {
      duree_precedente_envoi_trame_sec = 0;
    }
    else {
      duree_precedente_envoi_trame_sec = (duree_precedente_envoi_trame+500) / 1000; //+500 pour l'arrondi au dessus
    }
  }


  //Gestion heure et date
  String timeJsonTemp;
  String dateJsonTemp;
  if (rtc_datetime_ok == true) {
    timeJsonTemp = String(rtc.getHours()) + ":" + String(rtc.getMinutes()) + ":" + String(rtc.getSeconds());
    dateJsonTemp = String(rtc.getDay()) + "/" + String(rtc.getMonth()) + "/" + String(rtc.getYear());
    //Controle  time
    if (timeJsonTemp.substring(2,3) != ":") {
      timeJsonTemp = "0"+String(timeJsonTemp);
    }
    if (timeJsonTemp.substring(5,6) != ":") {
      timeJsonTemp = String(timeJsonTemp.substring(0,3)) + "0" + String(timeJsonTemp.substring(3,timeJsonTemp.length()));
    }
    if (timeJsonTemp.length() != 8) {
      timeJsonTemp = String(timeJsonTemp.substring(0,6)) + "0" + String(timeJsonTemp.substring(6,timeJsonTemp.length()));
    }
    timeJsonTemp.replace(":","");
    //Controle  Date
    if (dateJsonTemp.substring(2,3) != "/") {
      dateJsonTemp = "0"+String(dateJsonTemp);
    }
    if (dateJsonTemp.substring(5,6) != "/") {
      dateJsonTemp = String(dateJsonTemp.substring(0,3)) + "0" + String(dateJsonTemp.substring(3,dateJsonTemp.length()));
    }
    if (dateJsonTemp.length() != 8) {
      dateJsonTemp = String(dateJsonTemp.substring(0,6)) + "0" + String(dateJsonTemp.substring(6,dateJsonTemp.length()));
    }
    dateJsonTemp.replace("/","");
    dateJsonTemp = String(timeJsonTemp) + String(dateJsonTemp);
  }
  else {
    timeJsonTemp = "NULL";
  }
  
  

  if (type_trame == 0) {
    etat_trame_identification = false; //signifie que tant qu'elle n'a pas été envoyée, reste à false...
    trame_identification_a_envoyer = false; //passe à false car on vient de la générer
  }

  //Commun à tous les objets
  //IMEI
  trame_json["imei"] = imei;
  //PRODUIT
  trame_json["produit"] = typeObject;
  //TypeFrame
  trame_json["typeFrame"] = String(type_trame);
  
  if (mode_construction == "basique") {
    //Acquittement trame descendante
    trame_json["downR"] = String(downlinkStatus);

    //Configuration du reste de la trame en fonction du profil 
    if (typeObject == "EasyCompact") {
      if (type_trame == 0) {
        if (activateICCID == true) {
          trame_json["iccid"] = iccidNumber;
        }
        trame_json["imsi"] = imsiNumber;
        trame_json["fwSodaq"] = firmwareSodaq;
        trame_json["version"] = cardVersion + "-" + firmwareNumber;
        if (eeprom_variables == true) {
          trame_json["eeprom"] = "1";
        }
        else {
          trame_json["eeprom"] = "0";
        }
      }
      
      if ((type_trame == 0) or (type_trame == 1)) {
        //I1 => auto
        if (memo_i1 == false) {
          trame_json["i1"] = "1"; 
        }
        else {
          trame_json["i1"] = "0";
        }

        //I2 => 3/4
        if (memo_i2 == false) {
          trame_json["i2"] = "1"; 
        }
        else {
          trame_json["i2"] = "0";
        }

        //I3 => 4/4
        if (memo_i3 == false) {
          trame_json["i3"] = "1"; 
        }
        else {
          trame_json["i3"] = "0";
        }

        //I4 => compteur
        trame_json["i4"] = "0";

        //I5 => defaut
        if (memo_i5 == false) {
          trame_json["i5"] = "1"; 
        }
        else {
          trame_json["i5"] = "0";
        }

        //I6 => Arrêt d'urgence
        if (memo_i6 == false) {
          trame_json["i6"] = "1"; 
        }
        else {
          trame_json["i6"] = "0";
        }

        //I7 => Présence benne
        if (memo_i7 == false) {
          trame_json["i7"] = "1"; 
        }
        else {
          trame_json["i7"] = "0";
        }

        //I8 => Commutateur auto/mano
        if (memo_i8 == false) {
          trame_json["i8"] = "1"; 
        }
        else {
          trame_json["i8"] = "0";
        }

        //I9 => Non attribué
        if (memo_i9 == false) {
          trame_json["i9"] = "1"; 
        }
        else {
          trame_json["i9"] = "0";
        }
      }
      
      if ((type_trame == 2) or (type_trame == 3)) {
        if (latitude_gps == "") {
          latitude_gps = "NULL";
        }
        if (longitude_gps == "") {
          longitude_gps = "NULL";
        }
        if (nbre_satellite_gps == "") {
          nbre_satellite_gps = "0";
        }
        trame_json["GPS"][0] = latitude_gps;
        trame_json["GPS"][1] = longitude_gps;  
        trame_json["satellite"] = nbre_satellite_gps;
        if (((monobloc == false) and ((typeMove == 1) or (typeMove == 2))) or ((monobloc == true) and (typeMove != 0) and ((mode_gps == 0) or (mode_gps == 1)))) {
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println(F("Move = 0"));
          }
          typeMove = 0;
        }
        trame_json["move"] = String(typeMove);

        //MAJ typeMove = 2 que dans le cas où l'on détecte les positions intermédiaires
        if ((typeMove == 1) and (mode_gps == 3)) {
          typeMove = 2; 
        }
      }

      if (type_trame == 2) {
        trame_json["token"] = "life";
      }
      else if (type_trame == 3){
        trame_json["token"] = String(token);
        trame_json["typeFrame"] = "2"; //Update
      }



      
      
      if (memo_batterie == false) {
        trame_json["batterie"] = "1"; //Sur batterie
      }
      else {
        trame_json["batterie"] = "0"; //Sur secteur
      }
      
      trame_json["date"] = dateJsonTemp; 
    }
    
    else if (typeObject == "EasyPush") {
      if (type_trame == 3) {
        if (latitude_gps == "") {
          latitude_gps = "NULL";
        }
        if (longitude_gps == "") {
          longitude_gps = "NULL";
        }
        if (nbre_satellite_gps == "") {
          nbre_satellite_gps = "0";
        }
        trame_json["GPS"][0] = latitude_gps;
        trame_json["GPS"][1] = longitude_gps;  
        trame_json["satellite"] = nbre_satellite_gps;
      }
      else if (type_trame == 0) {
        if (activateICCID == true) {
          trame_json["iccid"] = iccidNumber;
        }
        trame_json["imsi"] = imsiNumber;
        trame_json["fwSodaq"] = firmwareSodaq;
        trame_json["version"] = cardVersion + firmwareNumber;
        if (eeprom_variables == true) {
          trame_json["eeprom"] = "1";
        }
        else {
          trame_json["eeprom"] = "0";
        }
      }
      
      //Nombre de bouton
      trame_json["nbrBtn"] = String(nombre_bouton);
      //Bouton 1
      if (etat_bouton_1 == false) {
        trame_json["i1"][0] = "0"; //["state"]
        trame_json["i1"][1] = String(str_type_alarme_bouton_1); //["alarm"]
      }
      else {
        trame_json["i1"][0] = "1"; //["state"]
        trame_json["i1"][1] = String(str_type_alarme_bouton_1); //["alarm"]
      }
      //Bouton 2
      if (etat_bouton_2 == false) {
        trame_json["i2"][0] = "0"; //["state"]
        trame_json["i2"][1] = String(str_type_alarme_bouton_2); //["alarm"]
      }
      else {
        trame_json["i2"][0] = "1"; //["state"]
        trame_json["i2"][1] = String(str_type_alarme_bouton_2); //["alarm"]
      }
      //Bouton 3
      if (etat_bouton_3 == false) {
        trame_json["i3"][0] = "0"; //["state"]
        trame_json["i3"][1] = String(str_type_alarme_bouton_3); //["alarm"]
      }
      else {
        trame_json["i3"][0] = "1"; //["state"]
        trame_json["i3"][1] = String(str_type_alarme_bouton_3); //["alarm"]
      }
      //Bouton 4
      if (etat_bouton_4 == false) {
        trame_json["i4"][0] = "0"; //["state"]
        trame_json["i4"][1] = String(str_type_alarme_bouton_4); //["alarm"]
      }
      else {
        trame_json["i4"][0] = "1"; //["state"]
        trame_json["i4"][1] = String(str_type_alarme_bouton_4); //["alarm"]
      }
      //Bouton 5
      if (etat_bouton_5 == false) {
        trame_json["i5"][0] = "0"; //["state"]
        trame_json["i5"][1] = String(str_type_alarme_bouton_5); //["alarm"]
      }
      else {
        trame_json["i5"][0] = "1"; //["state"]
        trame_json["i5"][1] = String(str_type_alarme_bouton_5); //["alarm"]
      }   
  
      trame_json["date"] = dateJsonTemp;
    }
    
    /*else if (typeObject == "EasyPush_Temp") { 
      trame_json["imsi"] = imsiNumber;
  
      if (bouton_flag_1 == true) {
        trame_json["benne"] = "1"; //Benne pleine
      }
      else if (bouton_flag_2 == true) {
        trame_json["benne"] = "0"; //Benne vide
      }
      else { //trame de vie
        if (numero_precedent_bouton == 1) {
          trame_json["benne"] = "1"; //Benne pleine
        }
        else if (numero_precedent_bouton == 2) {
          trame_json["benne"] = "0"; //Benne vide
        }
        else { //numero_precedent_bouton == 0
          trame_json["benne"] = "0"; //Benne vide
        }
      }
      trame_json["signal"] = String(networkSignal);
      trame_json["voltage"] = String(voltage_batterie);
  
      if (type_trame == 1) { //trame de vie avec GPS
        trame_json["GPS"][0] = latitude_gps;
        trame_json["GPS"][1] = longitude_gps;  
      }
      else {
        trame_json["GPS"][0] = "NULL";
        trame_json["GPS"][1] = "NULL";
      }
    }*/
    
    else { //typeObject == "premier_demarrage" or other...
      trame_json["typeTrame"] = "0";
      if (activateICCID == true) {
        trame_json["iccid"] = iccidNumber;
      }
      trame_json["imsi"] = imsiNumber;
      trame_json["fwSodaq"] = firmwareSodaq;
      trame_json["version"] = cardVersion + firmwareNumber;
      if (eeprom_variables == true) {
          trame_json["eeprom"] = "1";
      }
      else {
        trame_json["eeprom"] = "0";
      }
    }
  }
  
  else if (mode_construction == "reseau_7") {
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("Trame reseau_7"));
    }
    trame_json["token"] = String(token);
    trame_json["etat_rpg"] = "update";
    trame_json["apn7Value"] = String(apn7Value);
    if (foreignMode == false) {
      trame_json["ubandmask_7"] = String(ubandmask_7);
    }
    /*else {
      trame_json["ubandmask_etranger_7"] = String(ubandmask_etranger_7);
    }*/
  }

  else if (mode_construction == "reseau_8") {
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("Trame reseau_8"));
    }
    trame_json["token"] = String(token);
    trame_json["etat_rpg"] = "update";
    trame_json["apn8Value"] = String(apn8Value);
    if (foreignMode == false) {
      trame_json["ubandmask_8"] = String(ubandmask_8);
    }
    /*else {
      trame_json["ubandmask_etranger_8"] = String(ubandmask_etranger_8);
    }*/
  }

  else if (mode_construction == "reseau_78") {
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("Trame reseau_78"));
    }
    trame_json["token"] = String(token);
    trame_json["etat_rpg"] = "update";
    trame_json["apn78Value"] = String(apn78Value);
    if (foreignMode == false) {
      trame_json["ubandmask_7"] = String(ubandmask_7);
      trame_json["ubandmask_8"] = String(ubandmask_8);
    }
    /*else {
      trame_json["ubandmask_etranger_7"] = String(ubandmask_etranger_7);
      trame_json["ubandmask_etranger_8"] = String(ubandmask_etranger_8);
    }*/
  }

  else if (mode_construction == "serveur") {
    trame_json["token"] = String(token);
    trame_json["etat_rpg"] = "update";
    trame_json["adresse_serveur_mqtt"] = String(adresse_serveur_mqtt);
    trame_json["port_mqtt"] = String(port_mqtt);
    //trame_json["hote_mqtt"] = String(hote_mqtt);
    trame_json["pwd_mqtt"] = String(pwd_mqtt);
    trame_json["topic_filter_mqtt"] = String(topic_filter_mqtt);
    trame_json["topic_filter_mqtt_downlink"] = String(topic_filter_mqtt_downlink);
    trame_json["topic_filter_mqtt_reprog"] = String(topic_filter_mqtt_reprog);
  }  
  
  if (mode_construction == "basique") {
    trame_json["lastData"] = str_lastData;
    trame_json["dureeEnvoi"] = String(duree_precedente_envoi_trame_sec);
    trame_json["signal"] = String(networkSignal);
    trame_json["voltage"] = String(voltage_batterie);
    
    trame_json["compteur"] = String(compteur);
    //MAJ du compteur trame pour la prochaine
    compteur = compteur + 1;
  }
  else {
    trame_json["compteur"] = String(compteur_reprog);
    //MAJ du compteur trame pour la prochaine
    compteur_reprog = compteur_reprog + 1;
  }
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("Fin build trame"));
  }
}

void modification_trame_echec_avant_envoi(String topicUsed) {
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("\r\nStart modif trame save"));
    //DEBUG_STREAM.print(F("Trame en question : "));
    //serializeJson(trame_json, DEBUG_STREAM);
    //DEBUG_STREAM.print(F(""));
  }

  //ICCID
  if ((trame_json["typeFrame"] == "0") and (iccidARajouter == true)) {
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("ICCID AJOUTE !"));
    }
    iccidARajouter = false;
    trame_json["iccid"] = iccidNumber;
  }

  if (typeObject == "EasyPush_Temp") {
    //On modifie que le signal GSM
    trame_json["signal"] = String(networkSignal);
  }
  else {
    if (topicUsed == topic_filter_mqtt) {
      //Etat précédente trame
      trame_json["lastData"] = String(str_lastData);
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("MAJ str_lastData"));
      }

      //MAJ downlinkStatus
      if (downlinkStatus == "") {
        trame_json["downR"] = "noRead";
      }
      else {
        trame_json["downR"] = String(downlinkStatus);
      }
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println("MAJ downlinkStatus : "+String(downlinkStatus));
      }
    
      //Reset downlinkStatus
      downlinkStatus = "";
    }
      
    //le CSQ, signal réseau
    trame_json["signal"] = String(networkSignal);
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("MAJ signal"));
    }
    
    //Convert duree_precedente_envoi_trame
    if (topicUsed == topic_filter_mqtt) {
      unsigned long duree_precedente_envoi_trame_sec = 0;
      if (duree_precedente_envoi_trame == 0) {
        duree_precedente_envoi_trame_sec = 0;
      }
      else {
        duree_precedente_envoi_trame_sec = long((duree_precedente_envoi_trame+500) / 1000); 
      }
      trame_json["dureeEnvoi"] = String(duree_precedente_envoi_trame_sec);
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("MAJ dureeEnvoi"));
      }
    }
  }

  //Pompe
  if ((typeObject == "EasyCompact") and ((trame_json["typeFrame"] == "0") or (trame_json["typeFrame"] == "1"))) {
    if (timer_actif == true) {
      timer_pompe = timer_pompe + int((millis()-temps_debut_timer_pompe)/1000);
      temps_debut_timer_pompe = millis();
    }
    if (timer_pompe > 2) { //Pour éviter d'envoyer des temps très petits, si on le modifie, le modifier dans getVariable
      trame_json["i4"] = String(timer_pompe);
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("MAJ compteur i4")); //:"+String(trame_json["i4"].as<String>()));
      }
    } 
  }
}

void boucle_AT(unsigned long time_at) {
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("Start boucle AT"));
  }
  clignotement_led_easypush_temp_easycompact("cligno");
  unsigned long AT_currentMillis2 = millis();
  unsigned long AT_previousMillis2 = millis();
  AT_pass = false;
  while ((AT_pass == false) and (AT_currentMillis2 - AT_previousMillis2 <= time_at)) {
    //Controle pompe EasyCompact
    controle_pompe();
    
    clignotement_led_easypush_temp_easycompact("cligno");
    ATenvoi("AT");
    if(ATecoute("AT", "AT", 1000) == 1){ //2000
      AT_pass = true;
      sara_eveille = true;
    }
    clignotement_led_easypush_temp_easycompact("cligno");
    sodaq_wdt_safe_delay(200);
    AT_currentMillis2 = millis();
    //WatchDog Reset
    sodaq_wdt_reset();
  }
  if (AT_pass == false) {
    sara_eveille = false;
  }
}

void power_off_saraR4() {
  //Deconnexion propre du module SARA R4
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("\r\nStart Power Off Sara"));
  }

  bool deco_cpwroff = false;

  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println("d(enablePin) : "+String(digitalRead(enablePin)));
  }

  if ((firstStart == false) and (stateSara == true) and (sara_eveille == true) and (((statePSM == false) and (stateEDRX == false)) or (desactivatePsmFailed == true) or (desactivateEdrxFailed == true) or (extinction_force_ublox == true))) { //and (statePSM == false) ????????????
    ATenvoi("AT+CPWROFF");
    if (ATecoute("AT+CPWROFF", "AT+CPWROFF", 1000) == 1) {
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("SUCCESS DECO SARA"));
        //DEBUG_STREAM.println("Valeur tension enablePin : "+String(digitalRead(enablePin)));
      }
      deco_cpwroff = true;
      //Attente power_off_delay
      controle_pompe();
      clignotement_led_easypush_temp_easycompact("cligno");
      sodaq_wdt_safe_delay(1000);
      controle_pompe();
      clignotement_led_easypush_temp_easycompact("cligno");
    } 
  }

  
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println("Valeur tension enablePin : "+String(digitalRead(enablePin)));
  }


  //Kill serial Modem
  if ((firstStart == false)) { 
    MODEM_STREAM.flush();//.flush()//end();
  }
  

  if ((deco_cpwroff == true) and (digitalRead(enablePin) > 0)) {
    //Power pins to LOW
    if ((stateSara == true) and (((statePSM == false) and (stateEDRX == false)) or (desactivatePsmFailed == true) or (desactivateEdrxFailed == true) or (extinction_force_ublox == true))) {
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("OFF ALONE"));
      }

      unsigned long limit_timer = 10000;
      unsigned long temps_now_control = millis();
      while (millis() - temps_now_control < limit_timer) {
        if (digitalRead(enablePin) <= 0) { //enablePin
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println("Temps attendu pin low : "+String(millis() - temps_now_control));
          }
          
          temps_now_control = millis() + limit_timer + 1000;
        }
        else{
          sodaq_wdt_safe_delay(200);
          controle_pompe();
          clignotement_led_easypush_temp_easycompact("cligno");
          sodaq_wdt_reset();
        }
      }
      sodaq_wdt_safe_delay(50);
      pinMode(voltagePin, OUTPUT);
      digitalWrite(voltagePin, LOW);
      digitalWrite(powerPin, LOW); 
    }
  }
  else if ((deco_cpwroff == false) and (digitalRead(enablePin) > 0)) { //((digitalRead(voltagePin) > 0) or (digitalRead(enablePin) > 0))) {
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("DECO AUTRE QUE CPWROFF"));
    }
    pinMode(voltagePin, OUTPUT);
    digitalWrite(voltagePin, LOW);
    sodaq_wdt_safe_delay(2000); 
    pinMode(voltagePin, INPUT); 

    unsigned long limit_timer = 10000;
    unsigned long temps_now_control = millis();
    while (millis() - temps_now_control < limit_timer) {
      if (digitalRead(enablePin) <= 0) { //enablePin
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println("Temps attendu pin low : "+String(millis() - temps_now_control));
        }
        temps_now_control = millis() + limit_timer + 1000;
      }
      else{
        sodaq_wdt_safe_delay(200);
        controle_pompe();
        clignotement_led_easypush_temp_easycompact("cligno");
        sodaq_wdt_reset();
      }
    }

    sodaq_wdt_safe_delay(200);


    pinMode(voltagePin, OUTPUT);
    digitalWrite(voltagePin, LOW);
    digitalWrite(powerPin, LOW);
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println("digitalRead(enablePin) : "+String(digitalRead(enablePin)));
    }
  }
  else {
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("DEJA ETEINT"));
    }

    //Secu 
    sodaq_wdt_safe_delay(400);
    
    pinMode(voltagePin, OUTPUT);
    digitalWrite(voltagePin, LOW);
    digitalWrite(powerPin, LOW);
  }


  //Kill serial Modem
  if (firstStart == false) {
    MODEM_STREAM.flush();//end();
  }
  
  //MAJ variable etat
  stateSara = false; 
  sara_eveille = false;
  extinction_force_ublox = false;
  networkConnection = false;
}

void power_on_saraR4() {
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("\r\nDemarrage Power On Sara R4"));
  }

  no_return_at = false;

  controle_pompe();

  //Gestion des pins
  if (((statePSM == false) and (stateEDRX == false)) or (networkConnection == true) or (desactivatePsmFailed == true) or (desactivateEdrxFailed == true)) { 
    pinMode(voltagePin, OUTPUT);
    digitalWrite(powerPin, HIGH);
    digitalWrite(voltagePin, HIGH);
    
    
    sodaq_wdt_safe_delay(450);
  }

  pinMode(voltagePin, OUTPUT);
  digitalWrite(voltagePin, LOW);
  sodaq_wdt_safe_delay(500);  
  pinMode(voltagePin, INPUT);

  controle_pompe();
  clignotement_led_easypush_temp_easycompact("cligno");
  
  //if (statePSM == false) {
  MODEM_STREAM.begin(saraBaud);
  unsigned long now_millis = millis();
  while (!MODEM_STREAM && (millis() - now_millis < 10000)) {}

  if (!MODEM_STREAM) {
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println("\r\nEchec start Serial SARA ");
    }
  }
  
  controle_pompe();
  clignotement_led_easypush_temp_easycompact("cligno");

  sodaq_wdt_safe_delay(100); //Secu pour le modem
   
  //MAJ variable etat
  stateSara = true; 
}

void get_imei() {
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("\r\nDemarrage recup IMEI"));  
  }
  at_ok = 0;
  int tentative_get_imei = 0;
  int nbre_max_tentative_imei = 2;
  while ((at_ok == 0) and (tentative_get_imei < nbre_max_tentative_imei)) {
    AT_pass = false;
    boucle_AT(2000); //10 sec
    if (AT_pass == true) {
      ATResponseOK = true;
      ATenvoi("ATI");
      if (ATecoute("ATI", "IMEI:", 5000) == 1) {
        if (str_retour_at.length() > 0) {
          if ((str_retour_at.indexOf("IMEI:") > 0) and (str_retour_at.indexOf("Model:") > 0)) {
            //cardVersion
            cardVersion = str_retour_at.substring(str_retour_at.indexOf("Model:"),int(str_retour_at.length()));
            cardVersion = cardVersion.substring(cardVersion.indexOf("-")+1,int(cardVersion.length()));
            cardVersion = cardVersion.substring(0,cardVersion.indexOf("-"));
            
            //IMEI
            imei = str_retour_at.substring(str_retour_at.indexOf("IMEI:"),int(str_retour_at.length()));
            imei.remove(0,6);
            for (int i = 0; i < int(imei.length()); i++) {
              if (imei.substring(i,i+1) == "\r\n" or imei.substring(i,i+1) == " " or imei.substring(i,i+1) == "\r" or imei.substring(i,i+1) == "\n") {
                imei.remove(i);
                i = int(imei.length())+1;
              }
            } 
          }
        }
        else {
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println(F("Probleme longueur chaine de caractere"));
          }
          imei = "imei_error";
          cardVersion = "error";
          at_ok = 0;
        }
      }
    }
    else {
      ATResponseOK = false;
      at_ok = 0;
    }
    if (at_ok == 0) {
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("IMPOSSIBILITE RECUPERER IMEI"));
      }
      power_off_saraR4();
      sodaq_wdt_safe_delay(2000);
      imei = "imei_error";
      if (tentative_get_imei >= nbre_max_tentative_imei) {
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("Max tentative depasse"));
        }
      }
      else {
        power_on_saraR4();
      }
    }
    else {
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(">>> imei module SARA : "+String(imei));
        DEBUG_STREAM.println(">>> Version carte SODAQ : "+String(cardVersion));
      }
    }
    tentative_get_imei = tentative_get_imei + 1;
  }

  //MAJ du user MQTT
  if ((hote_mqtt == "") or (hote_mqtt == "imei_error")) {
    hote_mqtt = imei;
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("MAJ hote_mqtt par imei"));
    }
  }
  
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("\r\n"));
  }
}

void get_iccidNumber() {
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("\r\nDemarrage recup ICCID"));  
  }
  iccidNumber = "";
  at_ok = 0;
  int tentative_get_num_sim = 0;
  int nbre_max_tentative_num_sim = 2;
  while ((at_ok == 0) and (tentative_get_num_sim < nbre_max_tentative_num_sim)) {
    AT_pass = false;
    boucle_AT(2000); //10 sec
    if (AT_pass == true) {
      ATResponseOK = true;
      ATenvoi("AT+CCID");
      if (ATecoute("AT+CCID", "+CCID:", 5000) == 1) {
        if (str_retour_at.length() > 0) {
          iccidNumber = str_retour_at.substring(str_retour_at.indexOf("+CCID:"),int(str_retour_at.length()));
          iccidNumber.remove(0,7);
          for (int i = 0; i < int(iccidNumber.length()); i++) {
            if (iccidNumber.substring(i,i+1) == "\r\n" or iccidNumber.substring(i,i+1) == " " or iccidNumber.substring(i,i+1) == "\r" or iccidNumber.substring(i,i+1) == "\n") {
              iccidNumber.remove(i);
              i = int(iccidNumber.length())+1;
            }
          }
        }
        else {
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println(F("Probleme longueur chaine caractere"));
          }
          at_ok = 0;
          iccidNumber = "ccid_error";
        }
      }
    }
    else {
      ATResponseOK = false;
      at_ok = 0;
    }
    if (at_ok == 0) {
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("IMPOSSIBILITE RECUPERER ICCID"));
      }
      power_off_saraR4();
      sodaq_wdt_safe_delay(2000);
      iccidNumber = "ccid_error";
      if (tentative_get_num_sim >= nbre_max_tentative_num_sim) {
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("Nombre max de tentative depasse"));
        }
      }
      else {
        power_on_saraR4();
      }
    }
    else {
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(">>> numero ICCID : "+String(iccidNumber));
      }
    }
    tentative_get_num_sim = tentative_get_num_sim + 1;
  }
}

void get_firmwareSodaq() {
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("\r\nDemarrage recup firmware SODAQ"));  
  }
  at_ok = 0;
  int tentative_get_firmware = 0;
  int nbre_max_tentative_firmware = 2;
  while ((at_ok == 0) and (tentative_get_firmware < nbre_max_tentative_firmware)) {
    AT_pass = false;
    boucle_AT(2000); //10 sec
    if (AT_pass == true) {
      ATResponseOK = true;
      ATenvoi("ATI9");
      if (ATecoute("ATI9", "ATI9", 5000) == 1) {
        if (str_retour_at.length() > 0) {
          firmwareSodaq = str_retour_at.substring(str_retour_at.indexOf("ATI9"),str_retour_at.indexOf("OK"));
          firmwareSodaq.remove(0,4);
          //On supprime le OK
          firmwareSodaq.remove(firmwareSodaq.length()-2,firmwareSodaq.length());
          //On supprimes espaces
          for (int i = 0; i < int(firmwareSodaq.length()); i++) {
            if (firmwareSodaq.substring(i,i+1) != "\r\n" and firmwareSodaq.substring(i,i+1) != " " and firmwareSodaq.substring(i,i+1) != "" and firmwareSodaq.substring(i,i+1) != "\r" and firmwareSodaq.substring(i,i+1) != "\n") {
              if (i != 0) {
                firmwareSodaq.remove(0,i);
              }
              i = int(firmwareSodaq.length())+1;
            }
          }
          for (int i = 0; i < int(firmwareSodaq.length()); i++) {
            if (firmwareSodaq.substring(firmwareSodaq.length()-i,firmwareSodaq.length()-i-1) != "\r\n" and firmwareSodaq.substring(firmwareSodaq.length()-i,firmwareSodaq.length()-i-1) != " " and firmwareSodaq.substring(firmwareSodaq.length()-i,firmwareSodaq.length()-i-1) != "\r" and firmwareSodaq.substring(firmwareSodaq.length()-i,firmwareSodaq.length()-i-1) != "\n") {
              if (i != 0) {
                firmwareSodaq.remove(firmwareSodaq.length()-i,firmwareSodaq.length());
              }
              i = int(firmwareSodaq.length())+1;
            }
          }
        }
        else {
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println(F("Probleme longueur chaine de caractere"));
          }
          firmwareSodaq = "firmwareSodaq_error";
          at_ok = 0;
        }
      }
    }
    else {
      ATResponseOK = false;
      at_ok = 0;
    }
    if (at_ok == 0) {
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("IMPOSSIBILITE RECUPERER FIRMWARE SODAQ"));
      }
      power_off_saraR4();
      sodaq_wdt_safe_delay(1000);
      firmwareSodaq = "firmwareSodaq_error";
      if (tentative_get_firmware >= nbre_max_tentative_firmware) {
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("Nbre max tentative depasse"));
        }
      }
      else {
        power_on_saraR4();
      }
    }
    else {
      //On enlève les virgules
      firmwareSodaq.replace(",",":");
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(">>> version firmware module SARA : "+String(firmwareSodaq));
      }
    }
    tentative_get_firmware = tentative_get_firmware + 1;
  }
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("\r\n"));
  }
}

void gestion_boot_tension() {
  //Gestion boot
  if (firstStart == false) {
    //Activation du Serial
    if (DEBUG_SERIAL == true) {
      if (DEBUG_STREAM != SerialUSB) {
        //Fin serial
        gestion_serial("begin");
      }
      //Désactivation de l'alarme RTC
      DEBUG_STREAM.println(F("\r\n RTC OFF"));
    }
    //Detach interrupt alarm
    rtc.detachInterrupt();
    //Disable rtc alarm
    rtc.disableAlarm();
  }

  //Print message accueil
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("\r\nControle tension boot"));
  }

  //On recupere la tension de la batterie
  getBatteryVoltage();

  //On compare par rapport au seuil fixé
  if (voltage_batterie < tension_seuil) {
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("Tension inferieure seuil"));
    }
    if (flag_seuil_batterie == false) {
      flag_seuil_batterie = true;
    } 
    if (firstStart == true) {
      firstStart = false; 
    }
    goSleep = true;
  }
  else {
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("Tension superieure seuil"));
    }
    goSleep = false;
    flag_seuil_batterie = false;
    type_trame = 0;
    //Reprise du cycle firstStart
    firstStart = true; 
    //Reset value flag
    rtc_flag = false;
    bouton_flag_1 = false;
    bouton_flag_2 = false;
    bouton_flag_3 = false;
    bouton_flag_4 = false;
    bouton_flag_5 = false;
    bouton_flag_6 = false;
    acc_int_flag = false; 
    magInterrupt_flag = false;
  }

  //Print message fin

  //Power off si nécessaire avec alarme
  if (flag_seuil_batterie == true) {
    //On fixe la prochaine alarme
    initRtc(life_batterie*60);
    
    //Extinction serial
    if (DEBUG_SERIAL == true) {
      if (DEBUG_STREAM != SerialUSB) {
        //Fin serial
        gestion_serial("end");
      }
    }

    //Reset flag values
    rtc_flag = false;
    bouton_flag_1 = false;
    bouton_flag_2 = false;
    bouton_flag_3 = false;
    bouton_flag_4 = false;
    bouton_flag_5 = false;
    bouton_flag_6 = false;
    acc_int_flag = false; 
    magInterrupt_flag = false;    
  }
}


void getBatteryVoltage() {
  voltage_batterie = (uint16_t)((ADC_AREF / 1.023) * (BATVOLT_R1 + BATVOLT_R2) / BATVOLT_R2 * (float)analogRead(BATVOLT_PIN));
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println("Tension batterie : "+String(voltage_batterie));
  }
}

/*void statuts_mode_psm_r4() {
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("\r\nDemarrage statuts mode PSM"));
  }
  AT_pass = false;
  boucle_AT(2500); //10 sec
  controle_pompe();
  if (AT_pass == true) {
    ATenvoi("AT+CPSMS?");
    if (ATecoute("AT+CPSMS?", "+CPSMS:", 2000) == 1) {
      ATenvoi("AT+CEDRXS?");
      if (ATecoute("AT+CEDRXS?", "+CEDRXS:", 2000) == 1) {
        ATenvoi("AT+UPSV?");
        if (ATecoute("AT+UPSV?", "+UPSV:", 2000) == 1) {
          at_ok = 1;
        }
      }
    }
  } 
  controle_pompe();
}*/


/*void mode_edrx_r4(bool mode_edrx_statut) {
  if (mode_edrx_statut == true) {
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("\r\nDemarrage mode EDRX ON"));
    }
    AT_pass = false;
    boucle_AT(2500); //10 sec
    if (AT_pass == true) {
      if (DEBUG_SERIAL == true) {
        //DEBUG_STREAM.println(F("\r\nAMELIORER LE CONTROLE DE LA REPONSE !!!!!!!"));
      }
      if (apnvalue == apn7Value) { //Orange
        ATenvoi("AT+CEDRXS=2,4,\"1000\",\"0011\"");
        if (ATecoute("AT+CEDRXS=2,4,\"1000\",\"0011\"", "+CEDRXP:4,\"", 2000) == 1) {  //AMELIORER LE CONTROLE DE LA REPONSE !!!!!!!!!!!!!!!!!
          stateEDRX = true;
        }
      }
      else { //NB-IOT
        ATenvoi("AT+CEDRXS=2,5,");
        if (ATecoute("AT+CEDRXS=2,5,", "+CEDRXP:5,\"", 2000) == 1) { //AMELIORER LE CONTROLE DE LA REPONSE !!!!!!!!!!!!!!!!!
          stateEDRX = true;
        }
      }
    }
  }


  //A COMPLETER  !!!!!!!!
  
}*/


void mode_psm_r4(bool mode_psm_statut) {
  if (mode_psm_statut == false) {
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("\r\nDemarrage mode PSM OFF"));
    }
    AT_pass = false;
    boucle_AT(2500); //10 sec
    if (AT_pass == true) {
      ATenvoi("AT+CFUN=0");
      if (ATecoute("AT+CFUN=0", "AT+CFUN=0", 2000) == 1) {      
        ATenvoi("AT+CPSMS=0");
        if (ATecoute("AT+CPSMS=0", "AT+CPSMS=0", 2000) == 1) {
          ATenvoi("AT+UPSV=0");
          if (ATecoute("AT+UPSV=0", "AT+UPSV=0", 2000) == 1) {
            at_ok = 1;
            desactivatePsmFailed = false;
            statePSM = false;
          }
        }
      }
    } 
    if ((AT_pass == false) or (at_ok == 0)) {
      desactivatePsmFailed = true;
    }
  }
  //Desactivé pour faire de la place
  /*else {
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("\r\nDemarrage mode PSM ON"));
    }
    AT_pass = false;
    boucle_AT(2500); //10 sec
    if (AT_pass == true) {
      ATenvoi("AT+CPSMS=1,,,\"10000011\",\"00000001\"");
      if (ATecoute("AT+CPSMS=1,,,\"10000011\",\"00000001\"", "AT+CPSMS=1,,,\"10000011\",\"00000001\"", 2000) == 1) {
        statePSM = true;
      }
    }
  } */
}


void disable_psm() {
  ATenvoi("AT+UPSV?");
  if (ATecoute("AT+UPSV?", "AT+UPSV?", 2000) == 1) {
    if (val_retour_at == 0) {
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("Mode PSM a desactiver"));
      }
      ATenvoi("AT+CFUN=0");
      if (ATecoute("AT+CFUN=0", "AT+CFUN=0", 2000) == 1) {
        ATenvoi("AT+UPSV=0");
        if (ATecoute("AT+UPSV=0", "AT+UPSV=0", 2000) == 1) {
          ATenvoi("AT+CPSMS=0");
          if (ATecoute("AT+CPSMS=0", "AT+CPSMS=0", 2000) == 1) {
            ATenvoi("AT+UPSV?");
            if (ATecoute("AT+UPSV?", "AT+UPSV?", 2000) == 1) {
              if (val_retour_at == 1) {
                desactivatePsmFailed = false;
                statePSM = false;
              }
              else {
                desactivatePsmFailed = true;
              }
            }
            else {
              desactivatePsmFailed = true;
            }
          }
          else {
            desactivatePsmFailed = true;
          }
        }
        else {
          desactivatePsmFailed = true;
        }
      }
      else {
        desactivatePsmFailed = true;
      }
    }
    else {
      statePSM = false; 
      desactivatePsmFailed = false; 
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("Mode PSM deja OFF"));
      }
      ATenvoi("AT+CPSMS?");
      if (ATecoute("AT+CPSMS?", "AT+CPSMS?", 2000) == 1) {
        at_ok = 1;
      }
    }
  }
}

void disable_cedrxs() {
  desactivateEdrxFailed = true; //de base, ça évite de le mettre partout en dessous
  if (at_ok == 1) {
    val_retour_at = 0;
    ATenvoi("AT+CEDRXS?"); 
    if (ATecoute("AT+CEDRXS?", "AT+CEDRXS?", 5000) == 1) { 
      if (val_retour_at != 1) { 
        int tentative_urat = 0;
        val_retour_at = 0;
        ATenvoi("AT+CFUN=0"); 
        if (ATecoute("AT+CFUN=0", "AT+CFUN=0", 5000) == 1) {
          while ((val_retour_at != 1) and (tentative_urat < 1)) {                      
            ATenvoi("AT+CEDRXS=0,2"); 
            if (ATecoute("AT+CEDRXS=0,2", "AT+CEDRXS=0,2", 5000) == 1) { 
              ATenvoi("AT+CEDRXS=0,4"); 
              if (ATecoute("AT+CEDRXS=0,4", "AT+CEDRXS=0,4", 5000) == 1) {
                ATenvoi("AT+CEDRXS=0,5"); 
                if (ATecoute("AT+CEDRXS=0,5", "AT+CEDRXS=0,5", 5000) == 1) {    
                  ATenvoi("AT+CFUN=15"); 
                  if (ATecoute("AT+CFUN=15", "AT+CFUN=15", 5000) == 1) { 
                    if (stateSaraHello == false) {
                      sodaq_wdt_safe_delay(800);
                      boucle_AT(10000);
                    }
                    else {
                      //Ecoute hello sara
                      ecoute_demarrage_sara(temps_reponse_hello);
                    }
                      
                    if ((AT_pass == true) or (sara_eveille == true)) {        
                      unsigned long currentMillis3 = millis();
                      unsigned long previousMillis3 = millis();
                      at_ok = 0;
                      while ((at_ok == 0) and (currentMillis3 - previousMillis3 < 3000)) {
                        ATenvoi("AT+CPIN?"); //j'ai rajouté le ",1"
                        if (ATecoute("AT+CPIN?", "AT+CPIN?", 4000) == 1) { 
                          if (str_retour_at.indexOf("READY") > 0) {
                            at_ok = 1;
                          }
                          else {
                            if (DEBUG_SERIAL == true) {
                              DEBUG_STREAM.println(F("READY absent !"));
                            }
                            at_ok = 0;
                          }
                        }
                        else {
                          at_ok = 0;
                        } 
                        sodaq_wdt_safe_delay(300);
                        currentMillis3 = millis();               
                      }
                        
                      if (at_ok == 1) {
                        ATenvoi("AT");
                        if (ATecoute("AT", "AT", 1000) == 1) {
                          at_ok = 1;
                        }
                      }
                        
                      if (at_ok == 1) {
                        ATenvoi("AT+CEDRXS?");
                        if (ATecoute("AT+CEDRXS?", "AT+CEDRXS?", 5000) == 1) {
                          if (val_retour_at == 1) {
                            stateEDRX = false;
                            desactivateEdrxFailed = false;
                          }
                          else {
                            val_retour_at = 0;
                            if (DEBUG_SERIAL == true) {
                              DEBUG_STREAM.println(F("Erreur config CEDRXS"));
                            }
                            tentative_urat = tentative_urat + 1;
                            if (DEBUG_SERIAL == true) {
                              DEBUG_STREAM.println("New tentative : "+String(tentative_urat+1));
                            }
                            desactivateEdrxFailed = true;
                          }
                        }
                        else {
                          at_ok = 0;
                          tentative_urat = tentative_urat + 1;
                          if (DEBUG_SERIAL == true) {
                            DEBUG_STREAM.println("New tentative : "+String(tentative_urat+1));
                          }
                          desactivateEdrxFailed = true;
                        }
                      }
                    }  
                  }
                }
              }
            }
            tentative_urat = tentative_urat + 1;
          }
        }
      }
      else {
        stateEDRX = false;
        desactivateEdrxFailed = false;
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("eDRX deja OFF !"));
        }
      }
    }
  }
}


void ecoute_demarrage_sara(unsigned long delay_until) { //Mettre bien ce script juste après le power on de sara
  //On ecoute le module sara tant qu'il n'a pas demarre 
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("\r\nWait start SARA ...\r\nEcoute live : "));
  }
  reponse = "\n"; //Rajout de l espace afin de trouver par la suite la commande AT dans le message retour du module => c'est pour indexOf
  sara_eveille = false;
  durationStart = millis();
  while ((millis() - durationStart < delay_until) and (sara_eveille == false)) { 
    while (MODEM_STREAM.available()) { 
      reponse = reponse + char(MODEM_STREAM.read());
    }
    
    //Controle pompe EasyCompact
    controle_pompe();
    clignotement_led_easypush_temp_easycompact("cligno");
    
    if (reponse.indexOf(String(hello_sara)) > 0) {
      sara_eveille = true;
      if (stateSaraHello == false) {
        stateSaraHello = true;
      }
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(String(reponse));
        DEBUG_STREAM.println("\r\nTemps wait start SARA : " + String(millis() - durationStart));
      }
    }
  }

  if (sara_eveille == false) {
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("Hello non trouve !"));
    }
    stateSaraHello = false;
    
    //Boucle AT pour essayer de récupérer un AT dans le cas où le Hello Paprec n'est pas défini
    boucle_AT(2000);
    
    //On fixe le mode hello au demarrage
    if (sara_eveille == true) {
      fixe_demarrage_hello_sara();
    }
    else {
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("QUE FAIRE DANS CE CAS DE FIGURE ?"));
      }
    }
    
  }
  else {
    clignotement_led_easypush_temp_easycompact("cligno");
    ATenvoi("AT");
    if(ATecoute("AT", "AT", 1000) == 1){ 
      at_ok = 1;
    }
  }
}

void fixe_demarrage_hello_sara() {
  ATenvoi("AT+CSGT=1,\""+String(hello_sara)+"\"");
  if (ATecoute("AT+CSGT=1,\""+String(hello_sara)+"\"", "AT+CSGT=1,\""+String(hello_sara)+"\"", 2000) == 1) {
    stateSaraHello = true;
  }
  else {
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("Echec Hello Power ON !"));
    }
  }
}

void scenario_envoi_trame() {
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("\r\nDemarrage scenario_envoi_trame"));
    DEBUG_STREAM.println("Start tentative_trame : "+String(tentative_trame));
  }

  //Reset
  echec_envoi_trame_reveil = false;

  clignotement_led_easypush_temp_easycompact("loop");

  //Affichage des variables importantes :
  affichage_variables_importantes();

  //Controle pompe EasyCompact
  controle_pompe();

  //Desactivation du WDT
  //gestion_wdt("off"); //Desactiver ici pour V1.0.25

  //Envoi trame serveur si pas de changement de mode de la carte
  if (switchMode == false) { // and (((etat_degrade == false) and (at_ok == 1)) or (ATResponseOK == true))) {//((ATResponseOK == true) or (sara_eveille == true))) {
    clignotement_led_easypush_temp_easycompact("loop");
    //Pas de mode PSM
    if ((networkConnection == true) and (statePSM == false) and (stateEDRX == false) and (desactivatePsmFailed == false) and (desactivateEdrxFailed == false)) {
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("UBLOX allume"));
      }
      sentMQTT(false, true); 

      if (failConnexionReseau == true) {
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("\r\nECHEC START UBLOX MODE DEJA ALIMENTE => co classique"));
        }
        failConnexionReseau = false;
        connexion(); 
        if (connexion_ok == true) {
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println(F("\r\nDemarrage envoi Trame"));
          }
          sentMQTT(false, false); 
        }
      }
    }
    else if (((statePSM == false) and (stateEDRX == false)) or (desactivatePsmFailed == true) or (desactivateEdrxFailed == true)) {   
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("Demarrage connexion reseau IOT => PSM/eDRX OFF"));
      }
      connexion(); 
      if (connexion_ok == true) {
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("\r\nDemarrage envoi Trame"));
        }
        sentMQTT(false, false); 
      }
    }
    //Mode PSM activé
    else {
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("mode PSM/EDRX")); 
      }
      sentMQTT(true, false);  
    }
  }
  //Envoi trame en cas de changement de mode
  else if (switchMode == true) {
    //Mise a false de changement bool car pris en compte
    switchMode = false;

    clignotement_led_easypush_temp_easycompact("cligno");
    
    //Envoi trame de vie, fixer à 8 !!!
    type_trame = 1; //avant 8

    if ((returnServerSend == true) and (stateSara == true)) {
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("Demarrage envoi Trame"));
      }
      sentMQTT(false, true); //A REVOIR !!!!!!
      
    }
    else {
      if (stateSara == true) {
        clignotement_led_easypush_temp_easycompact("cligno");
        //Deconnexion réseau
        deconnexion();
        clignotement_led_easypush_temp_easycompact("cligno");
        //Power off sara
        if (((statePSM == false) and (stateEDRX == false)) or (desactivatePsmFailed == true) or (desactivateEdrxFailed == true)) {
          power_off_saraR4();
        }
      }

      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("Demarrage connexion reseau IOT => PSM OFF"));
      }
      connexion(); 

      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("Demarrage envoi Trame"));
      }
      sentMQTT(false, false); 
    }
  }

  //Controle pompe EasyCompact
  controle_pompe();
 

  //Changement etat de certaines variables par securite
  ATResponseOK = true;

  //Extinction des LEDS
  if (activateLED == true) {
    gestion_led("eteint"); 
  }
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("Fin scenation_envoi_trame"));
  }
}


void gestion_fin_envoi_trame() {
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("\r\nDebut gestion_fin_envoi_trame"));
  }

  //Controle pompe EasyCompact
  controle_pompe();

  //Gestion typeMove en cas de reprog et autre
  if (typeObject == "EasyCompact") {
    if (((mode_gps == 0) or (mode_gps == 1) or (monobloc == false)) and (typeMove != 0)) {
      typeMove = 0;        
    }
    else if ((typeMove == 3) and (monobloc == true)) {
      typeMove = 0;
    }
    else if ((typeMove == 2) and (monobloc == true)) {
      typeMove = 1;
    }
  }
    

  if ((switchMode == true)) {// and (retour_serveur == 1)) { //Il faut mettre retour_serveur == 1 car pour la trame reprog envoyee au cas où => reprog apres
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("Changement mode de la carte !"));
    }

    //Initialisation de certaines variables
    tentative_trame = 0;
    compteur = 0;
    type_trame = 0;
    trame_identification_a_envoyer = false;
    trame_necessite_reiteration = false;
    echec_envoi_trame_reveil = false;
    etat_trame_identification = false;


    //Desactivation du module SARA en cas d'echec envoi trame reprog
    if (returnServerSend == false) {
      if (stateSara == true) {
        //Deconnexion réseau
        deconnexion();
    
        //Power off sara
        if (((statePSM == false) and (stateEDRX == false)) or (desactivatePsmFailed == true) or (desactivateEdrxFailed == true)) {
          power_off_saraR4();
        }
      }
    }
    else if (stateSara == true) {
      power_off_saraR4();
    }
  }
  else {
    //Dodo activé
    goSleep = true;




    //Voir pour le tracker .... et voir dans SendTrame aussi
    if ((memo_batterie == true) and (typeObject == "EasyCompact") and (networkConnection == true) and (extinction_force_ublox == false)) {
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("Pas besoin d eteindre UBLOX !!!"));
      }
      deco_serveur_mqtt();
    }
    else if (stateSara == true) {
      //Deconnexion réseau
      deconnexion();
  
      //Power off sara
      if (((statePSM == false) and (stateEDRX == false)) or (desactivatePsmFailed == true) or (desactivateEdrxFailed == true) or (extinction_force_ublox == true)) {
        power_off_saraR4();
      }
    }



    

    //Activation du WDT
    //gestion_wdt("on"); //Desactiver ici pour V1.0.25


 
    if (echec_envoi_trame_reveil == true) {
      if (DEBUG_SERIAL == true) { 
        DEBUG_STREAM.println(F("Echec envoi trame reveil"));
      }
      
      echec_envoi_trame_reveil = false;

      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println("tentative_trame : "+String(tentative_trame));
      }
      
      if (((tentative_trame < max_essai_batterie) and (typeObject != "EasyCompact")) or ((typeObject == "EasyCompact") and (((memo_batterie == true) and (tentative_trame < max_essai_secteur)) or ((memo_batterie == false) and (tentative_trame < max_essai_batterie))))) {
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("Reste des tentatives ..."));
        }

        //Mode : Premier demarrage
        if (typeObject == "EasyStart") {
          trame_necessite_reiteration = true;

          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println("trame_necessite_reiteration  = "+String(trame_necessite_reiteration));
          }
          //On fixe la prochaine alarme
          initRtc(time_echec_iteration*60);
        }

        //Mode : EasyCompact
        else if (typeObject == "EasyCompact") { 
          if (((mode_gps == 2) and (typeMove == 1)) or ((mode_gps == 3) and (typeMove == 1)) or ((mode_gps == 3) and (typeMove == 2))) {
            trame_necessite_reiteration = false; 
          }
          else {
            trame_necessite_reiteration = true; 
          }
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println("trame_necessite_reiteration = "+String(trame_necessite_reiteration));
          }
          
          if (memo_batterie == false) {
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("Sur batterie"));
            }

            if (trame_necessite_reiteration == true) {
              if (time_echec_iteration == 0) {
                again_scenario = true;
                if (DEBUG_SERIAL == true) {
                  DEBUG_STREAM.println(F("again = true"));
                }
              }
              else {
                //Préparation alarme échec dans 15min
                initRtc(time_echec_iteration*60);
                if (DEBUG_SERIAL == true) {
                  DEBUG_STREAM.println(F("Alarme OK : time_echec_iteration"));
                }
                //Accel
                if (mode_gps == 2) {
                  accel_attach_loop();
                }
              }  
            }
            else {
              //Préparation alarme trame de vie
              if ((mode_gps == 0) or (mode_gps == 1) or (monobloc == false)) {
                initRtc(life_compact_batterie*60);
              }
              else { //mode_gps == 2
                if (typeMove == 1) {
                  //DISABLE ACCEL au cas où
                  if (acc_int_flag == true) {
                    disable_accelerometre();
                  }
                  //RTC
                  initRtc(life_move_dodo*60);
                }
                else { //typeMove == 3 ou 0
                  accel_attach_loop();
                  initRtc(life_compact_batterie*60);
                }
              }
            }
            
            //Préparation des boutons
            attach_interrupt_bouton();
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("Attach batterie OK"));  
            }
          }
          else {
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("Secteur"));
            }
            if (trame_necessite_reiteration == true) {
              if (time_echec_secteur == 0) {
                again_scenario = true;
                if (DEBUG_SERIAL == true) {
                  DEBUG_STREAM.println(F("again = true"));
                }
              }             
            }

            clignotement_led_easypush_temp_easycompact("result");
          }
        }
        
        //Mode : bouton 5 + 1
        else if (typeObject == "EasyPush") {
          //Trames de vie ou de bouton
          trame_necessite_reiteration = true;

          //Préparation alarme échec dans 15min
          initRtc(time_echec_iteration*60);
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println(F("Alarme OK"));
          }

          //Préparation des boutons
          attach_interrupt_bouton();
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println(F("Boutons OK"));
          }
        }

        //Mode : bouton Temp
        else if (typeObject == "EasyPush_Temp") {
          //Trames de vie ou de bouton
          trame_necessite_reiteration = true;

          //Préparation alarme échec dans 15min
          initRtc(time_echec_iteration*60);
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println(F("Alarme OK"));
          }
          
          //Préparation des boutons
          attach_interrupt_bouton();
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println(F("Boutons OK"));
          }

          //LED
          clignotement_led_easypush_temp_easycompact("echec");
        }
      }
      else {
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("Nombre max tentative depasse !"));
        }
        
        //Mise a zero
        tentative_trame = 0;
        trame_necessite_reiteration = false;
        echec_envoi_trame_reveil = false;



        //On supprime les trames stockées en échec
        if ((nombre_echec_envoi_trame > 0) and (suppTab == true)) {
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println("Suppression trame echec n° : "+String(nombre_echec_envoi_trame));
          }
          trame_echec[nombre_echec_envoi_trame-1] = "";
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println("Ancien nombre trame echec : "+String(nombre_echec_envoi_trame));
          }
          nombre_echec_envoi_trame = nombre_echec_envoi_trame - 1;
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println("Nouveau nombre trame echec : "+String(nombre_echec_envoi_trame));
          }          

          if ((str_lastData == "errCSQ") and (signal_ok == false)) {
            str_lastData = "errCSQSupp";
          }
          else if ((str_lastData == "errServeur") and (serverConnection == false)) {
            str_lastData = "errServeurSupp";
          }
          else if (str_lastData == "errEnvoi") {
            str_lastData = "errEnvoiSupp";
          }
          else if (str_lastData == "errProfil") {
            str_lastData = "errProfilSupp";
          }
          else if (str_lastData == "errUblox") {
            str_lastData = "errUbloxSupp";
          }
          else {
            str_lastData = "errSupp";
          }
           
          if (etat_trame_identification == false) {
            etat_trame_identification = true; //On remet à True
            trame_identification_a_envoyer = true;
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("trame_identification_a_envoyer = TRUE car trame ID delete !"));
            }
          }
        }



           
        //Mode : Premier demarrage
        if (typeObject == "EasyStart") {
          //On regarde s'il reste des trames à envoyer
          if ((nombre_echec_envoi_trame != 0) and (max_essai_batterie > 1)) {
            //MAJ variables
            tentative_trame = nombre_echec_envoi_trame;
            trame_necessite_reiteration = true;
            
            //Préparation alarme vie dans 15min
            initRtc(time_echec_iteration*60);
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("Alarme OK"));
            }
          }
          else {
            //On fixe la prochaine alarme
            initRtc(life_start*60);
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("Alarme OK"));
            }
            trame_necessite_reiteration = false;
          }
        }

        //Mode : EasyCompact
        else if (typeObject == "EasyCompact") { 
          //On regarde s'il reste des trames à envoyer
          /*if (((nombre_echec_envoi_trame != 0) and (suppTab == true)) and (((max_essai_batterie > 1) and (memo_batterie == false)) or ((max_essai_secteur > 1) and (memo_batterie == true)))) {
            //MAJ variables
            if ((mode_gps == 2) and (typeMove == 1)) {
              tentative_trame = 0;
              trame_necessite_reiteration = false;
            }
            else {
              tentative_trame = nombre_echec_envoi_trame;
              trame_necessite_reiteration = true;
              
              if (DEBUG_SERIAL == true) {
                DEBUG_STREAM.println("trame_necessite_reiteration = true et tentative_trame : "+String(tentative_trame));
              }
            }
              
            
            if (memo_batterie == false) {
              if (DEBUG_SERIAL == true) {
                DEBUG_STREAM.println(F("Sur batterie !"));
              }
              if (trame_necessite_reiteration == true) {
                if (time_echec_iteration == 0) {
                  again_scenario = true;
                  if (DEBUG_SERIAL == true) {
                    DEBUG_STREAM.println(F("again = true"));
                  }
                }
                else {
                  //Préparation alarme échec dans 15min
                  initRtc(time_echec_iteration*60);
                  if (DEBUG_SERIAL == true) {
                    DEBUG_STREAM.println(F("Alarme OK : time_echec_iteration"));
                  }
                  //ACCEL
                  if ((mode_gps == 2) and (monobloc == true))  {
                    accel_attach_loop();
                  }
                }   
              }
              else {
                if ((mode_gps == 0) or (mode_gps == 1) or (monobloc == false)) {
                  initRtc(life_compact_batterie*60);
                }
                else { //mode_gps == 2
                  if (typeMove == 1) {
                    //DISABLE ACCEL au cas où
                    if (acc_int_flag == true) {
                      disable_accelerometre();
                    }
                    //RTC
                    initRtc(life_move_dodo*60);
                  }
                  else { //typeMove == 3 ou 0
                    accel_attach_loop();
                    initRtc(life_compact_batterie*60);  
                  }
                }
              }
              //Préparation des boutons
              attach_interrupt_bouton();
            }
            else {
              if (DEBUG_SERIAL == true) {
                DEBUG_STREAM.println(F("Secteur"));
              }
              if (trame_necessite_reiteration == true) {
                again_scenario = true;
                if (DEBUG_SERIAL == true) {
                  DEBUG_STREAM.println(F("again = true"));
                }
              }
              
              clignotement_led_easypush_temp_easycompact("result");
            }
          }*/
          //else {
          trame_necessite_reiteration = false;
          
          if (memo_batterie == false) {
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("Sur batterie bis"));
            }

            if ((mode_gps == 0) or (mode_gps == 1) or (monobloc == false)) {
              //Préparation alarme trame de vie
              initRtc(life_compact_batterie*60);
            }
            else { //mode_gps == 2
              if (typeMove == 1) {
                //DISABLE ACCEL au cas où
                if (acc_int_flag == true) {
                  disable_accelerometre();
                }
                //RTC
                initRtc(life_move_dodo*60);
              }
              else { //typeMove == 3 ou 0
                accel_attach_loop();
                initRtc(life_compact_batterie*60);
              }
            }     
            //Préparation des boutons
            attach_interrupt_bouton();
          }
          else {
            clignotement_led_easypush_temp_easycompact("result");
          }
          //}
        }

        //Mode : bouton 5 + 1
        else if (typeObject == "EasyPush") {     
          //On regarde s'il reste des trames à envoyer
          if ((nombre_echec_envoi_trame != 0) and (max_essai_batterie > 1)) {
            //MAJ variables
            tentative_trame = nombre_echec_envoi_trame;
            trame_necessite_reiteration = true;
            
            //Préparation alarme vie dans 15min
            initRtc(time_echec_iteration*60);
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("Alarme OK"));
            }
          }
          else {
            //Préparation alarme RTC
            if ((resetServeur == true) and ((etat_bouton_1 == true) or (etat_bouton_2 == true) or (etat_bouton_3 == true) or (etat_bouton_4 == true) or (etat_bouton_5 == true))) {
              if ((rtc.getEpoch() - time0LifeBouton) < ((life_bouton * 60) - (life_reset * 60))) { 
                initRtc(life_reset*60);
              }
              else {
                initRtc((life_bouton *60) - (rtc.getEpoch() - time0LifeBouton));
              }
            }
            else {
              //Préparation alarme vie dans 1440min (24h)
              initRtc(life_bouton*60);
            }
            
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("Alarme OK"));
            }
            trame_necessite_reiteration = false;
          }
          
          //Préparation des boutons
          attach_interrupt_bouton();
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println(F("Boutons OK"));
          }
        }
        
        //Mode : bouton Temp
        else if (typeObject == "EasyPush_Temp") {
          //Trames de vie ou de bouton
          //On regarde s'il reste des trames à envoyer
          if ((nombre_echec_envoi_trame != 0) and (max_essai_batterie > 1)) {
            //MAJ variables
            tentative_trame = nombre_echec_envoi_trame;
            trame_necessite_reiteration = true;
            
            //Préparation alarme vie dans 15min
            initRtc(time_echec_iteration*60);
          }
          else {
            //Préparation alarme vie dans 1440min (24h)
            initRtc(life_bouton*60);
            trame_necessite_reiteration = false;
          }
          
          //Préparation des boutons
          attach_interrupt_bouton();
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println(F("Boutons OK"));
          }

          //LED
          clignotement_led_easypush_temp_easycompact("echec");
        }  
      }
    }
    else {
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("Trame reveil send OK !"));
      }
      //Changement etat variables
      if (etat_trame_identification == false) { 
        etat_trame_identification = true;
        trame_identification_a_envoyer = false;
      }

      //Mode : Premier demarrage
      if (typeObject == "EasyStart") {
        initRtc(life_start*60);
      }

      //Mode : EasyCompact 
      else if (typeObject == "EasyCompact") { 
        if (memo_batterie == false) {
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println(F("Sur batterie ON"));
          }

          if ((mode_gps == 0) or (mode_gps == 1) or (monobloc == false)) {
            //Préparation alarme trame de vie
            initRtc(life_compact_batterie*60);
          }
          else { //mode_gps == 2
            if (typeMove == 1) {
              //DISABLE ACCEL au cas où
              if (acc_int_flag == true) {
                disable_accelerometre();
              }
              //RTC
              initRtc(life_move_dodo*60);
            }
            else { //typeMove == 3 ou 0
              accel_attach_loop();
              initRtc(life_compact_batterie*60);
            }
          }
          //Préparation des boutons
          attach_interrupt_bouton();
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println(F("Attach batterie OK"));  
          }        
        }
        else {
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println(F("Secteur => no alarm")); 
          }
          clignotement_led_easypush_temp_easycompact("result");
        }       
      }
      
      //Mode : bouton 5+1 boutons
      else if (typeObject == "EasyPush") {
        //Trames de vie ou de bouton
        //Préparation alarme RTC
        if ((resetServeur == true) and ((etat_bouton_1 == true) or (etat_bouton_2 == true) or (etat_bouton_3 == true) or (etat_bouton_4 == true) or (etat_bouton_5 == true))) {
          if ((rtc.getEpoch() - time0LifeBouton) < ((life_bouton * 60) - (life_reset * 60))) { 
            initRtc(life_reset*60);
          }
          else {
            initRtc((life_bouton *60) - (rtc.getEpoch() - time0LifeBouton));
          }
        }
        else {
          //Préparation alarme échec dans 1440min (24h)
          initRtc(life_bouton*60);
        }
        
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("Alarme OK"));
        }
        
        //Préparation des boutons
        attach_interrupt_bouton();
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("Boutons OK"));
        }
      } 

      
      //Mode : bouton Temp
      else if (typeObject == "EasyPush_Temp") {
        //Trames de vie ou de bouton
        //Préparation alarme échec dans 1440min (24h)
        initRtc(life_bouton*60);
        
        //Préparation des boutons
        attach_interrupt_bouton();
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("Boutons OK"));
        }

        //LED
        clignotement_led_easypush_temp_easycompact("success");
      } 

      //Reset valeur
      tentative_trame = 0;
      trame_necessite_reiteration = false;
    }    
  }

  if (extinction_force_ublox == true) {
    extinction_force_ublox = false;
  }
  
  //Affichage des variables importantes
  //affichage_variables_importantes();

  //Controle pompe EasyCompact
  controle_pompe();

  //Enregistrement temps seconde du moment où le module s'endort
  temps_seconde_module_dodo = rtc.getEpoch();

  //Enregistrement temps pour alarme de vie en mode secteur
  temps_last_trame = millis();

  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("Fin gestion_fin_envoi_trame"));
  }
}


void gestion_led(String couleur_led) {
  if (couleur_led == "eteint") {
    digitalWrite(led_red, HIGH);
    digitalWrite(led_green, HIGH);
    digitalWrite(led_blue, HIGH);
    stateLedSodaq = false;
  }
  else if (couleur_led == "bleu") { //
    digitalWrite(led_red, HIGH);
    digitalWrite(led_green, HIGH);
    digitalWrite(led_blue, LOW);
    stateLedSodaq = true;
  }
  else if (couleur_led == "rouge") { //
    digitalWrite(led_blue, HIGH);
    digitalWrite(led_green, HIGH);
    digitalWrite(led_red, LOW);
    stateLedSodaq = true;
  }
  else if (couleur_led == "vert") { //
    digitalWrite(led_blue, HIGH);
    digitalWrite(led_red, HIGH);
    digitalWrite(led_green, LOW); 
    stateLedSodaq = true;  
  }
  else if (couleur_led == "blanc") { //
    digitalWrite(led_red, LOW);
    digitalWrite(led_green, LOW);
    digitalWrite(led_blue, LOW);
    stateLedSodaq = true;
  }
  else if (couleur_led == "jaune") { //
    digitalWrite(led_blue, HIGH);
    digitalWrite(led_red, LOW);
    digitalWrite(led_green, LOW); 
    stateLedSodaq = true;
  }
  else if (couleur_led == "violet") { //
    digitalWrite(led_green, HIGH);
    digitalWrite(led_red, LOW);
    digitalWrite(led_blue, LOW);
    stateLedSodaq = true;
  }
  else if (couleur_led == "cyan") { //
    digitalWrite(led_red, HIGH);
    digitalWrite(led_blue, LOW);
    digitalWrite(led_green, LOW); 
    stateLedSodaq = true;
  }
  else {
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("\r\nCouleur inconnue !"));
    }
  }
}

void sauvegarde_trame_echec_reprog() {
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("\r\nDemarrage sauvegarde trame reprog"));
  }

  //Save json dans Tab
  int taille_tableau_save_json_retour = int(sizeof(trame_reprog_new)/sizeof(trame_reprog_new[0])); 
  if (compteur_trame_reprog_new >= taille_tableau_save_json_retour) {
    //veut dire que l'on a plus de place dans le tableau de json !!! Normalement on ne peut pas entraîner ici, on suffisamment de place
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("Plus de place !"));  //Pas possible normalement
    }

    for (int i = 0; i < compteur_trame_reprog_new; i++) {
      trame_reprog_new[i] == "";
    }

    //Reset du coup du compteur
    compteur_trame_reprog_new = 0;
  }

  serializeJson(trame_json, trame_reprog_new[compteur_trame_reprog_new]);
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println("Affichage json enregistre : "+String(trame_reprog_new[compteur_trame_reprog_new]));
  }
  compteur_trame_reprog_new = compteur_trame_reprog_new + 1;

  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println("compteur_trame_reprog_new : "+String(compteur_trame_reprog_new));
    DEBUG_STREAM.println(F("Affichage tab save trames echecs reprog :"));
    for (int i = 0; i < int(sizeof(trame_reprog_new)/sizeof(trame_reprog_new[0])); i++) {
      DEBUG_STREAM.println("Position "+String(i)+ " : "+String(trame_reprog_new[i]));
    }
  }
}

void sauvegarde_trame_echec() {
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("\r\nDemarrage sauvegarde trame non envoyee"));
    //DEBUG_STREAM.println("Size tab trame_echec : "+String(sizeof(trame_echec)/sizeof(trame_echec[0]))); //DEBUG
    DEBUG_STREAM.println("nombre_echec_envoi_trame : "+String(nombre_echec_envoi_trame)); //DEBUG
  }

  if (nombre_echec_envoi_trame != 0) { // and (nombre_echec_envoi_trame <= (sizeof(trame_echec)/sizeof(trame_echec[0])))) {
    for (int i = nombre_echec_envoi_trame; i >= 0; i--) {
      if (i == 0) {
        trame_echec[i] = "";
        serializeJson(trame_json, trame_echec[i]);
      }
      else if (i < int(sizeof(trame_echec)/sizeof(trame_echec[0]))) {
        trame_echec[i] = trame_echec[i-1];
      }
      else {
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("1 trame lost !"));
        }

        if ((str_stateData == "errCSQ") and (signal_ok == false)) {
          str_stateData = "errCSQSupp";
        }
        else if ((str_stateData == "errServeur") and (serverConnection == false)) {
          str_stateData = "errServeurSupp";
        }
        else if (str_stateData == "errEnvoi") {
          str_stateData = "errEnvoiSupp";
        }
        else if (str_stateData == "errProfil") {
          str_stateData = "errProfilSupp";
        }
        else if (str_lastData == "errUblox") {
          str_lastData = "errUbloxSupp";
        }
        else {
          str_stateData = "errSupp";
        }

        
        
        if (etat_trame_identification == false) {
          etat_trame_identification = true; //On remet à True
          trame_identification_a_envoyer = true;
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println(F("trame_identification_a_envoyer = TRUE car trame ID delete !"));
          }
        }
      }
    }
    //MAJ du nbre de trame en echec a envoyer
    if (nombre_echec_envoi_trame < int(sizeof(trame_echec)/sizeof(trame_echec[0]))) {
      nombre_echec_envoi_trame = nombre_echec_envoi_trame + 1;
    }
    else if (nombre_echec_envoi_trame > int(sizeof(trame_echec)/sizeof(trame_echec[0]))) {
      nombre_echec_envoi_trame = int(sizeof(trame_echec)/sizeof(trame_echec[0]));
    }
  }
  else { //else if (nombre_echec_envoi_trame == 0) {
    serializeJson(trame_json, trame_echec[0]);

    //MAJ du nbre de trame en echec a envoyer
    nombre_echec_envoi_trame = 1;
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("New trame save en 0 !"));
    }
  }

  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println("nombre_echec_envoi_trame : "+String(nombre_echec_envoi_trame));
    DEBUG_STREAM.println(F("Affichage tab save trames echecs :"));
    for (int i = 0; i < int(sizeof(trame_echec)/sizeof(trame_echec[0])); i++) {
      DEBUG_STREAM.println("Position "+String(i)+ " : "+String(trame_echec[i]));
    }
  }
}

void decalage_trame_echec_tab() {
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("\r\nDemarrage decalage tab trame non send"));
    DEBUG_STREAM.println("nombre_echec_envoi_trame avant modif : "+String(nombre_echec_envoi_trame));
  }
  //On abaisse le nbre de trame en echec en attente d'envoi
  nombre_echec_envoi_trame = nombre_echec_envoi_trame - 1;

  trame_echec[nombre_echec_envoi_trame] = "";

  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println("nombre_echec_envoi_trame : "+String(nombre_echec_envoi_trame));
    DEBUG_STREAM.println(F("Affichage tab save des trames echecs :"));
    for (int i = 0; i < int(sizeof(trame_echec)/sizeof(trame_echec[0])); i++) {
      DEBUG_STREAM.println("Position "+String(i)+ " : "+String(trame_echec[i]));
    }
  }
}

void modif_trame_echec_tracker_gps() {
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("\r\nDemarrage modif trame tracker gps erreur"));
  }

  //MAJ de la trame echec
  if (nombre_echec_envoi_trame > 0) {
    //Conversion en Json pour modification
    trame_json.clear(); //Clear Json
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println("trame_echec avant modification : "+String(trame_echec[nombre_echec_envoi_trame-1]));
    }
    DeserializationError error = deserializeJson(trame_json, trame_echec[nombre_echec_envoi_trame-1]);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
    }
    else {
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.print(F("deserializeJson() NO error: "));
        DEBUG_STREAM.println(String(error.f_str()));
      }
      deserializeJson(trame_json, trame_echec[nombre_echec_envoi_trame-1]);
      //MAJ Heure GPS
      if ((heure_gps != "error") and (heure_gps.length() != 0)) {
        trame_json["heureGPS"] = String(heure_gps);
      }
      //MAJ Date GPS
      if ((date_gps != "error") and (date_gps.length() != 0)) {
        trame_json["dateGPS"] = String(date_gps);
      }
      //MAJ Latitude GPS
      if ((latitude_gps != "error") and (latitude_gps.length() != 0)) {
        trame_json["GPS"][0] = latitude_gps;
      }
      //MAJ Longitude GPS
      if ((longitude_gps != "error") and (longitude_gps.length() != 0)) {
        trame_json["GPS"][1] = longitude_gps; 
      }
      //MAJ Vitesse GPS
      if ((vitesse_kmh_gps != "error") and (vitesse_kmh_gps.length() != 0)) {
        trame_json["vitesseKmh"] = String(vitesse_kmh_gps);
      }
      //MAJ nbre_satellite_gps
      if ((nbre_satellite_gps != "error") and (nbre_satellite_gps.length() != 0)) {
        trame_json["satellite"] = nbre_satellite_gps;
      }

      //Enregistrement de la trame MAJ
      serializeJson(trame_json, trame_echec[nombre_echec_envoi_trame-1]);
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println("New trame_echec[] : "+String(trame_echec[nombre_echec_envoi_trame-1]));
      }
    }
  }
}

void affichage_variables_importantes() {
  //Affichage des variables importantes
  if (DEBUG_SERIAL == true) {
    /*DEBUG_STREAM.println("\r\nValeur des variables importantes :");
    DEBUG_STREAM.println("bouton_flag_1 : "+String(bouton_flag_1));
    DEBUG_STREAM.println("bouton_flag_2 : "+String(bouton_flag_2));
    DEBUG_STREAM.println("bouton_flag_3 : "+String(bouton_flag_3));
    DEBUG_STREAM.println("bouton_flag_4 : "+String(bouton_flag_4));
    DEBUG_STREAM.println("bouton_flag_5 : "+String(bouton_flag_5));
    DEBUG_STREAM.println("bouton_flag_6 : "+String(bouton_flag_6));
    DEBUG_STREAM.println("batterie_flag : "+String(batterie_flag));
    DEBUG_STREAM.println("rtc_flag : "+String(rtc_flag));
    DEBUG_STREAM.println("acc_int_flag : "+String(acc_int_flag));
    DEBUG_STREAM.println("magInterrupt_flag : "+String(magInterrupt_flag));
    DEBUG_STREAM.println("switchMode : "+String(switchMode));
    DEBUG_STREAM.println("firstStart : "+String(firstStart));
    DEBUG_STREAM.println("statePSM : "+String(statePSM));
    DEBUG_STREAM.println("stateEDRX : "+String(stateEDRX));
    DEBUG_STREAM.println("desactivatePsmFailed : "+String(desactivatePsmFailed));
    DEBUG_STREAM.println("desactivateEdrxFailed : "+String(desactivateEdrxFailed));
    DEBUG_STREAM.println("echec_envoi_trame_reveil : "+String(echec_envoi_trame_reveil));
    DEBUG_STREAM.println("trame_necessite_reiteration : "+String(trame_necessite_reiteration));
    DEBUG_STREAM.println("trame_identification_a_envoyer : "+String(trame_identification_a_envoyer));
    DEBUG_STREAM.println("type_trame : "+String(type_trame));
    DEBUG_STREAM.println("tentative_trame : "+String(tentative_trame));
    
    DEBUG_STREAM.println(F("///////////////////////////////////\r\n")); */
  }
}

void controle_move() {
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("\r\nDemarrage controle move"));
  }
  
  //Reset valeur
  mouvement_check_ok = false;

  if ((typeMove == 0) or (typeMove == 1)) {//(type_trame == 8) {
    //On controle par le magnetometre en premier lieu
    controle_move_magneto();
    if (mouvement_check_ok == true) {
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("On est bien en move !"));
      }
    }
    else {
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("Faux move detecte"));
      }
    }
  }
  else {
    //On controle par le GPS
    position_GPS("gps", timeout_gps_move, false, true);
    analyse_mouvement_gps("accel");  

    //On controle par le magneto si echec GPS
    /*if ((mouvement_check_ok == false) and (vitesse_kmh_gps == "error")) {
      controle_move_magneto();
    }
    else {
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("Besoin inutile controle par magneto car on a reusssi a checker le move par le GPS !"));
      }
    }*/
  }

  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println("mouvement_check_ok : "+String(mouvement_check_ok));
  }
}


void clignotement_led_easypush_temp_easycompact(String mode_cligno) {
  /*if (typeObject == "EasyPush_Temp") {
    if ((mode_cligno == "cligno") or (mode_cligno == "gps") or (mode_cligno == "loop")) {
      if (etat_led_easypush_temp == true) {
        if (millis() - memo_time_led >= 2000) { //Si éteint plus de 2 sec on éteint   
          memo_time_led = millis();
          etat_led_easypush_temp = false;
          digitalWrite(led_push_temp, LOW);
        }
      }
      else {
        if (memo_time_led == 0) { //Pour le démarrage du mode cligno
          memo_time_led = millis();
          etat_led_easypush_temp = true;
          digitalWrite(led_push_temp, HIGH);
        }
        else {
          if (millis() - memo_time_led >= 1000) { //Si éteint plus de 1 sec on allume
            memo_time_led = millis();
            etat_led_easypush_temp = true;
            digitalWrite(led_push_temp, HIGH);
          }
        }
      }
    }
    else if (mode_cligno == "success") {
      if (etat_led_easypush_temp == true) {
        digitalWrite(led_push_temp, LOW);
        sodaq_wdt_safe_delay(1000);
      }
      else {
        while (millis() - memo_time_led < 500) {
          sodaq_wdt_safe_delay(50);
        }
      }
      digitalWrite(led_push_temp, HIGH);
      sodaq_wdt_safe_delay(3000);
      digitalWrite(led_push_temp, LOW);
      etat_led_easypush_temp = false;
      memo_time_led = 0;
    }
    else if (mode_cligno == "echec") {
      if (etat_led_easypush_temp == true) {
        digitalWrite(led_push_temp, LOW);
        sodaq_wdt_safe_delay(1000);
      }
      else {
        while (millis() - memo_time_led < 500) {
          sodaq_wdt_safe_delay(50);
        }
      } 
      for (int i = 0; i < 3; i++) {
        if (i != 0) {
          sodaq_wdt_safe_delay(300);
        }
        digitalWrite(led_push_temp, HIGH);
        sodaq_wdt_safe_delay(300);
        digitalWrite(led_push_temp, LOW);
      } 
      etat_led_easypush_temp = false;
      memo_time_led = 0;
    }
    else { // mode_cligno == "btn_deja_ok"
      digitalWrite(led_push_temp, HIGH);
      sodaq_wdt_safe_delay(3000);
      digitalWrite(led_push_temp, LOW);
      etat_led_easypush_temp = false;   
    }
  }*/
  if ((typeObject == "EasyCompact") and (memo_batterie == true)) {
    if (mode_cligno == "cligno") {
      if ((stateLedSodaq == false) or (memo_time_led == 0)) {
        if ((memo_time_led == 0) or (millis() - memo_time_led > 3000)) { 
          gestion_led("bleu");
          memo_time_led = millis();
        }
      }
      else {
        if (millis() - memo_time_led > 1000) {
          gestion_led("eteint");
          memo_time_led = millis();
        }
      }
    }
    else if (mode_cligno == "gps") {
      if ((stateLedSodaq == false) or (memo_time_led == 0)) {
        if ((memo_time_led == 0) or (millis() - memo_time_led > 3000)) { 
          gestion_led("violet");
          memo_time_led = millis();
        }
      }
      else {
        if (millis() - memo_time_led > 1000) {
          gestion_led("eteint");
          memo_time_led = millis();
        }
      }
    }
    else if (mode_cligno == "result") { 
      if (returnServerSend == true) {
        gestion_led("vert");
      }
      else {
        gestion_led("rouge");
      }
      ledTransition = true;
      
      memo_time_led = millis();
    }
    else { //Mode loop hors envoi
      if (firstStart == false) {
        if (ledTransition == true) {
          if (millis() - memo_time_led > 5000) {
            gestion_led("eteint");
            memo_time_led = millis();
            ledTransition = false;
          }
        }
        else {
          if (stateLedSodaq == false) {
            if ((millis() - memo_time_led > 3000) and (returnServerSend == true)) {
              gestion_led("vert");
              memo_time_led = millis();
            }
            else if ((millis() - memo_time_led > 1000) and (returnServerSend == false)) {
              gestion_led("rouge");
              memo_time_led = millis();
            }          
          }
          else {
            if (millis() - memo_time_led > 1000) {
              gestion_led("eteint");
              memo_time_led = millis();
            }
          }
        }
      }
    }
  }
}

void controle_auto_reset_bouton() {
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("\r\nDemarrage controle_auto_reset_bouton"));
  }

  bool changement_etat_benne_pleine = false; //variable qui atteste si reset d'une benne

  if (time_reset_bouton != 0) { //Si time_reset_bouton est nul, cela désactive la fonctionnalité de reset automatique
    //Controle duree etat benne 1 pleine
    if ((time_push_bouton_1 != 0) and (rtc.getEpoch() - time_push_bouton_1 >= (time_reset_bouton*3600))) {
      time_push_bouton_1 = 0;
      etat_bouton_1 = false;
      str_type_alarme_bouton_1 = "reset";
      changement_etat_benne_pleine = true;
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("Benne 1 reset : vide !"));
      }
    }
    //Controle duree etat benne 2 pleine
    if ((time_push_bouton_2 != 0) and (rtc.getEpoch() - time_push_bouton_2 >= (time_reset_bouton*3600))) {
      time_push_bouton_2 = 0;
      etat_bouton_2 = false;
      str_type_alarme_bouton_2 = "reset";
      changement_etat_benne_pleine = true;
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("Benne 2 reset : vide !"));
      }
    }
    //Controle duree etat benne 3 pleine
    if ((time_push_bouton_3 != 0) and (rtc.getEpoch() - time_push_bouton_3 >= (time_reset_bouton*3600))) {
      time_push_bouton_3 = 0;
      etat_bouton_3 = false;
      str_type_alarme_bouton_3 = "reset";
      changement_etat_benne_pleine = true;
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("Benne 3 reset : vide !"));
      }
    }
    //Controle duree etat benne 4 pleine
    if ((time_push_bouton_4 != 0) and (rtc.getEpoch() - time_push_bouton_4 >= (time_reset_bouton*3600))) {
      time_push_bouton_4 = 0;
      etat_bouton_4 = false;
      str_type_alarme_bouton_4 = "reset";
      changement_etat_benne_pleine = true;
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("Benne 4 reset : vide !"));
      }
    }
    //Controle duree etat benne 5 pleine
    if ((time_push_bouton_5 != 0) and (rtc.getEpoch() - time_push_bouton_5 >= (time_reset_bouton*3600))) {
      time_push_bouton_5 = 0;
      etat_bouton_5 = false;
      str_type_alarme_bouton_5 = "reset";
      changement_etat_benne_pleine = true;
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println(F("Benne 5 reset : vide !"));
      }
    }
  
    //Preparation de la trame 
    if (changement_etat_benne_pleine == true) {
      //Type trame : bouton   
      byte save_type_trame = type_trame;
      type_trame = 2;
  
      //Construction de la trame
      construction_trame("basique");
  
      //Sauvegarder la trame dans les échecs pour être envoyé après
      sauvegarde_trame_echec();

      if (str_stateData == "errSupp") {
        str_lastData = str_stateData;
      }
  
      //Réinitialisation tentative trame 
      trame_necessite_reiteration = true;
  
      //reset type_trame
      type_trame = save_type_trame;
    }
  }
  else {
    if (DEBUG_SERIAL == true) {
      DEBUG_STREAM.println(F("Auto reset bouton non actif car 0"));
    }
  }
}

void controle_pompe() {
  if (typeObject == "EasyCompact") {
    if (memo_i4 != digitalRead(pin_i4)) {
      memo_i4 = digitalRead(pin_i4);
      if (memo_i4 == false) {
        temps_debut_timer_pompe = millis();
        timer_actif = true;
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("\r\nPOMPE EN FONCTIONNEMENT\r\n"));
        }
      }
      else {
        timer_actif = false;
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("\r\nPOMPE STOP"));
          DEBUG_STREAM.println("OLD timer_pompe en sec : "+String(timer_pompe));
        }
        timer_pompe = timer_pompe + int((millis()-temps_debut_timer_pompe)/1000);
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println("Temps pompe fctionnement save en sec : "+String(int((millis()-temps_debut_timer_pompe)/1000)));
          DEBUG_STREAM.println("New timer_pompe en sec : "+String(timer_pompe)+"\r\n");
        }
        temps_debut_timer_pompe = 0;
      }
    }
  } 
}

void controle_presence_virgule_json() {
  //Controle presence virgule dans les valeurs du json
  if (DEBUG_SERIAL == true) {
    DEBUG_STREAM.println(F("\r\nDemarrage controle virgule json"));
  }

  //Conversion du JSON
  JsonObject trame_json_objet = trame_json.as<JsonObject>();
  for (JsonPair kv : trame_json_objet) {
    String valeur_modifier = "";
    String valeur_modifier_1 = "";
    if ((String(kv.key().c_str()) == "GPS") or ((typeObject == "EasyPush") and ((String(kv.key().c_str()) == "i1") or (String(kv.key().c_str()) == "i2") or (String(kv.key().c_str()) == "i3") or (String(kv.key().c_str()) == "i4") or (String(kv.key().c_str()) == "i5")))) {
      valeur_modifier = trame_json[String(kv.key().c_str())][0].as<String>();
      valeur_modifier_1 = trame_json[String(kv.key().c_str())][1].as<String>();
    }
    else {
      valeur_modifier = trame_json[String(kv.key().c_str())].as<String>();
    }
    
    bool virgule_presente = false;
    while (valeur_modifier.indexOf(",") > 0) {
      valeur_modifier.replace(",","?");
      virgule_presente = true;
    }
    if (virgule_presente == true) {
      if (DEBUG_SERIAL == true) {
        DEBUG_STREAM.println("Virgule presente, nouvelle valeur : "+String(valeur_modifier));
      }
      if (valeur_modifier_1 != "") {
        trame_json[String(kv.key().c_str())][0] = valeur_modifier;
      }
      else {
        trame_json[String(kv.key().c_str())] = valeur_modifier;
      }
    }

    if (valeur_modifier_1 != "") {
      virgule_presente = false;
      while (valeur_modifier_1.indexOf(",") > 0) {
        valeur_modifier_1.replace(",","?");
        virgule_presente = true;
      }
      if (virgule_presente == true) {
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println("Virgule presente, nouvelle valeur : "+String(valeur_modifier_1));
        }
        trame_json[String(kv.key().c_str())][1] = valeur_modifier_1;
      }
    }
    
    sodaq_wdt_reset();
  }
}

void controle_9_entree_easycompact() {
  //Declaration des variables
  type_trame = 62;
  unsigned long memo_time_i1 = 0;
  unsigned long memo_time_i2 = 0;
  unsigned long memo_time_i3 = 0;
  unsigned long memo_time_i5 = 0;
  unsigned long memo_time_batterie = 0;
  unsigned long memo_time_i6 = 0;
  unsigned long memo_time_i7 = 0;
  unsigned long memo_time_i8 = 0;
  unsigned long memo_time_i9 = 0;
  bool save_i1 = memo_i1;
  bool save_i2 = memo_i2;
  bool save_i3 = memo_i3;
  bool save_i5 = memo_i5;
  bool save_batterie = memo_batterie;
  bool save_i6 = memo_i6;
  bool save_i7 = memo_i7;
  bool save_i8 = memo_i8;
  bool save_i9 = memo_i9;
  bool controle_en_cours = true; //laisser à true

  bool stopControlI1 = false; //Ne pas toucher
  bool stopControlI2 = false; //Ne pas toucher
  bool stopControlI3 = false; //Ne pas toucher
  bool stopControlI5 = false; //Ne pas toucher
  bool stopControlBatterie = false; //Ne pas toucher
  bool stopControlI6 = false; //Ne pas toucher
  bool stopControlI7 = false; //Ne pas toucher
  bool stopControlI8 = false; //Ne pas toucher
  bool stopControlI9 = false; //Ne pas toucher

  while (controle_en_cours == true) {
    // Reset watchdog
    sodaq_wdt_reset();
    
    //I1 : PIN AUTO
    if ((memo_i1 != digitalRead(pin_i1)) and (controle_en_cours == true) and (stopControlI1 == false)) {
      if (memo_time_i1 == 0) {
        save_i1 = digitalRead(pin_i1);
        memo_time_i1 = millis();
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("\r\nEntree I1 AUTO controle ..."));
        }
      }
      else {
        if (((millis() - memo_time_i1 >= (tmpUp1 * 1000)) and (save_i1 == false)) or ((millis() - memo_time_i1 >= (tmpDown1 * 1000)) and (save_i1 == true))) {
          stopControlI1 = true;
          if (type_trame != 2) {
            type_trame = 1;
          }
          if (save_i1 == false) {
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("\r\nAUTO I1 ON"));
            }
          }
          else {
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("\r\nAUTO I1 OFF")); 
            }
          }
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println("Temps wait Millis : "+String(millis() - memo_time_i1));
          }
          memo_time_i1 = 0; //Reset pour sortir cycle
        }
      }
    }
    else if (stopControlI1 == false) {
      if (memo_time_i1 != 0) {
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println("\r\nFin Controle I1 AUTO\r\nTemps wait entree I1 AUTO Millis : "+String(millis() - memo_time_i1));
        }
        memo_time_i1 = 0; //Reset pour sortir cycle  
        save_i1 = memo_i1;
      }
    }

    //I2 : PIN 3/4
    if ((memo_i2 != digitalRead(pin_i2)) and (controle_en_cours == true) and (stopControlI2 == false)) {
      if (memo_time_i2 == 0) {
        save_i2 = digitalRead(pin_i2);
        memo_time_i2 = millis();
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("\r\nEntree I2 3/4 controle ..."));
        }
      }
      else {
        if (((millis() - memo_time_i2 >= (tmpUp2 * 1000)) and (save_i2 == false)) or ((millis() - memo_time_i2 >= (tmpDown2 * 1000)) and (save_i2 == true))) {
          stopControlI2 = true;
          if (type_trame != 2) {
            type_trame = 1;
          }
          if (save_i2 == false) {
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("\r\nEntree I2 3/4 ON"));
            }
          }
          else {
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("\r\nEntree I2 3/4 OFF")); 
            }
          }
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println("Temps wait Millis : "+String(millis() - memo_time_i2));
          }
          memo_time_i2 = 0; //Reset pour sortir cycle
        }
      }
    }
    else if (stopControlI2 == false) {
      if (memo_time_i2 != 0) {
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println("\r\nFin Controle I2 3/4\r\nTemps wait entree I2 3/4 Millis : "+String(millis() - memo_time_i2));
        }
        memo_time_i2 = 0; //Reset pour sortir cycle  
        save_i2 = memo_i2;
      }
    }

    //I3 : PIN 4/4
    if ((memo_i3 != digitalRead(pin_i3)) and (controle_en_cours == true) and (stopControlI3 == false)) {
      if (memo_time_i3 == 0) {
        save_i3 = digitalRead(pin_i3);
        memo_time_i3 = millis();
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("\r\nEntree I3 4/4 controle ..."));
        }
      }
      else {
        if (((millis() - memo_time_i3 >= (tmpUp3 * 1000)) and (save_i3 == false)) or ((millis() - memo_time_i3 >= (tmpDown3 * 1000)) and (save_i3 == true))) {
          stopControlI3 = true;
          if (type_trame != 2) {
            type_trame = 1;
          }
          if (save_i3 == false) {
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("\r\nEntree I3 4/4 ON"));
            }
          }
          else {
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("\r\nEntree I3 4/4 OFF")); 
            }
          }
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println("Temps wait Millis : "+String(millis() - memo_time_i3));
          }
          memo_time_i3 = 0; //Reset pour sortir cycle
        }
      }
    }
    else if (stopControlI3 == false) {
      if (memo_time_i3 != 0) {
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println("\r\nFin Controle I3 4/4\r\nTemps wait entree I3 4/4 Millis : "+String(millis() - memo_time_i3));
        }
        memo_time_i3 = 0; //Reset pour sortir cycle  
        save_i3 = memo_i3;
      }
    }

    //I5 : PIN DEFAUT
    if ((memo_i5 != digitalRead(pin_i5)) and (controle_en_cours == true) and (stopControlI5 == false)) {
      if (memo_time_i5 == 0) {
        save_i5 = digitalRead(pin_i5);
        memo_time_i5 = millis();
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("\r\nEntree I5 defaut controle ..."));
        }
      }
      else {
        if (((millis() - memo_time_i5 >= (tmpUp5 * 1000)) and (save_i5 == false)) or ((millis() - memo_time_i5 >= (tmpDown5 * 1000)) and (save_i5 == true))) {
          stopControlI5 = true;
          if (type_trame != 2) {
            type_trame = 1;
          }
          if (save_i5 == false) {
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("\r\nEntree I5 DEFAUT ON"));
            }
          }
          else {
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("\r\nEntree I5 DEFAUT OFF")); 
            }
          }
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println("Temps wait Millis : "+String(millis() - memo_time_i5));
          }
          memo_time_i5 = 0; //Reset pour sortir cycle
        }
      }
    }
    else if (stopControlI5 == false) {
      if (memo_time_i5 != 0) {
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println("\r\nFin Controle I5 DEFAUT\r\nTemps wait entree I5 DEFAUT Millis : "+String(millis() - memo_time_i5));
        }
        memo_time_i5 = 0; //Reset pour sortir cycle  
        save_i5 = memo_i5;
      }
    }  

    //PIN BATTERIE
    if ((memo_batterie != digitalRead(pin_batterie)) and (controle_en_cours == true) and (stopControlBatterie == false)) {
      if (memo_time_batterie == 0) {
        save_batterie = digitalRead(pin_batterie);
        memo_time_batterie = millis();
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("\r\nEntree batterie controle ..."));
        }
      }
      else {
        if (millis() - memo_time_batterie >= (tmpBat * 1000)) {
          stopControlBatterie = true;
          type_trame = 2;
          memo_batterie = save_batterie; 
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println(F("\r\nOn est sur batterie !"));
          }
          
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println("Temps wait Millis : "+String(millis() - memo_time_batterie));
          }
          memo_time_batterie = 0; //Reset pour sortir cycle
          controle_en_cours = false; //pour sortir
        }
      }
    }
    else if (stopControlBatterie == false) {
      if (memo_time_batterie != 0) {
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println("\r\nFin Controle batterie\r\nTemps wait entree BATTERIE Millis : "+String(millis() - memo_time_batterie));
        }
        memo_time_batterie = 0; //Reset pour sortir cycle  
      }
    }

    //I6 : PIN ARRET D'URGENCE
    if ((memo_i6 != digitalRead(pin_i6)) and (controle_en_cours == true) and (stopControlI6 == false)) {
      if (memo_time_i6 == 0) {
        save_i6 = digitalRead(pin_i6);
        memo_time_i6 = millis();
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("\r\nEntree I6 arret d'urgence controle ..."));
        }
      }
      else {
        if (((millis() - memo_time_i6 >= (tmpUp6 * 1000)) and (save_i6 == false)) or ((millis() - memo_time_i6 >= (tmpDown6 * 1000)) and (save_i6 == true))) {
          stopControlI6 = true;
          if (type_trame != 2) {
            type_trame = 1;
          }
          if (save_i6 == false) {
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("\r\nEntree I6 ARRET D'URGENCE ON"));
            }
          }
          else {
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("\r\nEntree I6 ARRET D'URGENCE OFF")); 
            }
          }
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println("Temps wait Millis : "+String(millis() - memo_time_i6));
          }
          memo_time_i6 = 0; //Reset pour sortir cycle
        }
      }
    }
    else if (stopControlI6 == false) {
      if (memo_time_i6 != 0) {
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println("\r\nFin Controle I6 ARRET D'URGENCE\r\nTemps wait entree I6 ARRET D'URGENCE Millis : "+String(millis() - memo_time_i6));
        }
        memo_time_i6 = 0; //Reset pour sortir cycle  
        save_i6 = memo_i6;
      }
    } 

    //I7 : PIN PRESENCE BENNE
    if ((memo_i7 != digitalRead(pin_i7)) and (controle_en_cours == true) and (stopControlI7 == false)) {
      if (memo_time_i7 == 0) {
        save_i7 = digitalRead(pin_i7);
        memo_time_i7 = millis();
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("\r\nEntree I7 presence benne controle ..."));
        }
      }
      else {
        if (((millis() - memo_time_i7 >= (tmpUp7 * 1000)) and (save_i7 == false)) or ((millis() - memo_time_i7 >= (tmpDown7 * 1000)) and (save_i7 == true))) {
          stopControlI7 = true;
          if (type_trame != 2) {
            type_trame = 1;
          }
          if (save_i7 == false) {
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("\r\nEntree I7 PRESENCE BENNE ON"));
            }
          }
          else {
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("\r\nEntree I7 PRESENCE BENNE OFF")); 
            }
          }
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println("Temps wait Millis : "+String(millis() - memo_time_i7));
          }
          memo_time_i7 = 0; //Reset pour sortir cycle
        }
      }
    }
    else if (stopControlI7 == false) {
      if (memo_time_i7 != 0) {
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println("\r\nFin Controle I7 PRESENCE BENNE\r\nTemps wait entree I7 PRESENCE BENNE Millis : "+String(millis() - memo_time_i7));
        }
        memo_time_i7 = 0; //Reset pour sortir cycle  
        save_i7 = memo_i7;
      }
    } 
    
    //I8 : PIN COMMUTATEUR AUTO/MANO
    if ((memo_i8 != digitalRead(pin_i8)) and (controle_en_cours == true) and (stopControlI8 == false)) {
      if (memo_time_i8 == 0) {
        save_i8 = digitalRead(pin_i8);
        memo_time_i8 = millis();
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("\r\nEntree I8 commutateur auto/mano ..."));
        }
      }
      else {
        if (((millis() - memo_time_i8 >= (tmpUp8 * 1000)) and (save_i8 == false)) or ((millis() - memo_time_i8 >= (tmpDown8 * 1000)) and (save_i8 == true))) {
          stopControlI8 = true;
          if (type_trame != 2) {
            type_trame = 1;
          }
          if (save_i8 == false) {
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("\r\nEntree I8 COMMUTATEUR AUTO/MANO ON"));
            }
          }
          else {
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("\r\nEntree I8 COMMUTATEUR AUTO/MANO OFF")); 
            }
          }
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println("Temps wait Millis : "+String(millis() - memo_time_i8));
          }
          memo_time_i8 = 0; //Reset pour sortir cycle
        }
      }
    }
    else if (stopControlI8 == false) {
      if (memo_time_i8 != 0) {
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println("\r\nFin Controle I8 COMMUTATEUR AUTO/MANO\r\nTemps wait entree I8 COMMUTATEUR AUTO/MANO Millis : "+String(millis() - memo_time_i8));
        }
        memo_time_i8 = 0; //Reset pour sortir cycle  
        save_i8 = memo_i8;
      }
    } 

    //I9 : PIN NON ATTRIBUE
    if ((memo_i9 != digitalRead(pin_i9)) and (controle_en_cours == true) and (stopControlI9 == false)) {
      if (memo_time_i9 == 0) {
        save_i9 = digitalRead(pin_i9);
        memo_time_i9 = millis();
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println(F("\r\nEntree I9 supplementaire ..."));
        }
      }
      else {
        if (((millis() - memo_time_i9 >= (tmpUp9 * 1000)) and (save_i9 == false)) or ((millis() - memo_time_i9 >= (tmpDown9 * 1000)) and (save_i9 == true))) {
          stopControlI9 = true;
          if (type_trame != 2) {
            type_trame = 1;
          }
          if (save_i9 == false) {
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("\r\nEntree I9 SUPPLEMENTAIRE ON"));
            }
          }
          else {
            if (DEBUG_SERIAL == true) {
              DEBUG_STREAM.println(F("\r\nEntree I9 SUPPLEMENTAIRE OFF")); 
            }
          }
          if (DEBUG_SERIAL == true) {
            DEBUG_STREAM.println("Temps wait Millis : "+String(millis() - memo_time_i9));
          }
          memo_time_i9 = 0; //Reset pour sortir cycle
        }
      }
    }
    else if (stopControlI9 == false) {
      if (memo_time_i9 != 0) {
        if (DEBUG_SERIAL == true) {
          DEBUG_STREAM.println("\r\nFin Controle entree I9 SUPPLEMENTAIRE\r\nTemps wait entree I9 SUPPLEMENTAIRE Millis : "+String(millis() - memo_time_i9));
        }
        memo_time_i9 = 0; //Reset pour sortir cycle  
        save_i9 = memo_i9;
      }
    } 

    if ((memo_time_i1 == 0) and (memo_time_i2 == 0) and (memo_time_i3 == 0) and (memo_time_i5 == 0) and (memo_time_batterie == 0) and (memo_time_i6 == 0) and (memo_time_i7 == 0) and (memo_time_i8 == 0) and (memo_time_i9 == 0)) {
      controle_en_cours = false; //Pour sortir
    }

    //Controle pompe
    controle_pompe();

    //Jeu Led Sodaq
    clignotement_led_easypush_temp_easycompact("entree");
  }

  //MAJ des memo des entrées !!
  if (type_trame == 1) {
    memo_i1 = save_i1; 
    memo_i2 = save_i2; 
    memo_i3 = save_i3; 
    memo_i5 = save_i5; 
    memo_i6 = save_i6; 
    memo_i7 = save_i7; 
    memo_i8 = save_i8; 
    memo_i9 = save_i9; 
  }
}
