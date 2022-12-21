#!/bin/sh

  echo; apt install qtbase5-dev libgl1-mesa-dev libxtst-dev libx11-dev qtconnectivity5-dev libqt5svg5-dev alien apt-file wget coreutils libicu-dev;
  
  echo; apt-file update; echo;
 
 
  lib64jpeg8="http://abf-downloads.openmandriva.org/rolling/repository/x86_64/main/release/lib64jpeg8-2.1.4-1-omv4090.x86_64.rpm";
  wget -q -c $lib64jpeg8 -O ./lib64jpeg8.rpm;

  lib64png12_0="http://abf-downloads.openmandriva.org/rolling/repository/x86_64/main/release/lib64png12_0-1.2.59-1-omv4090.x86_64.rpm";
  wget -q -c $lib64png12_0 -O ./lib64png12-0.rpm;


  echo; mkdir -vp paquete/DEBIAN paquete/usr/bin/ paquete/usr/local/bin/ paquete/etc/xdg/autostart; echo;
  if [ -d "WifiMouseServer" ]; then rm -r WifiMouseServer; fi;
  
  
  debian_package="https://github.com/krogank9/WifiMouseServer/releases/download/1/wifimouseserver.deb";
  wget -q -c $debian_package -O ./wifimouseserver.deb;
  
  dpkg-deb -x wifimouseserver.deb paquete;
  dpkg-deb -e wifimouseserver.deb paquete/DEBIAN;


  if [ ! -f "paquete/DEBIAN/postinst" ]; then touch paquete/DEBIAN/postinst; fi;
  if [ ! -f "paquete/DEBIAN/postrm" ]; then touch paquete/DEBIAN/postrm; fi;
  if [ ! -f "paquete/DEBIAN/compat" ]; then touch paquete/DEBIAN/compat; fi;
   

  for x in *.rpm; do alien --to-deb --scripts $x 2>/dev/null; name=$(echo $x | sed -e 's,.rpm,,g'); dpkg-deb -x ${name}*.deb paquete/; done;
  for x in *.rpm; do name=$(echo $x | sed -e 's,.rpm,,g'); rm ${name}*; done;
    
  find paquete/usr/share/doc/*/changelog.Debian.gz wifimouseserver.deb | xargs rm

  
  source_code="https://github.com/michaelnpsp/WifiMouseServer";
  echo; git clone $source_code; echo;

  wget -q -c https://raw.githubusercontent.com/seba1452021/WifiMouseServer/master/logo.png -O - | cat > WifiMouseServer/images/icon64.png;
  
  
  cd WifiMouseServer;
  export LD_LIBRARY_PATH="$(pwd)/../paquete/usr/lib64";

  qmake; echo; make -j$(nproc); 


  libraries=$(ldd WifiMouseServer | awk '/=>/ {print $3}' | sort | uniq);
  potential_owners=$(for x in $libraries; do sudo apt-file search "$x " | cut -d ':' -f 1; done | sort | uniq);
  sures_owners=$(for y in $potential_owners; do sudo apt list --installed $y 2>/dev/null; done | awk '/ins/ {print $1}' | cut -d '/' -f 1);
  list=$(echo $sures_owners | sed -e 's# #, #g');


  printf "Depends: $list \nSection: net \nPriority: optional \n" | tee -a $(pwd)/../paquete/DEBIAN/control 1>/dev/null;
 
  printf "#! /bin/sh \n sleep 7 && echo 'Execute: sudo ufw allow 9798/tcp';" | tee $(pwd)/../paquete/DEBIAN/post* 1>/dev/null;
 
  echo '10' > $(pwd)/../paquete/DEBIAN/compat;
  
  sed -i 's,allow,deny,g' $(pwd)/../paquete/DEBIAN/postrm 1>/dev/null;
  
  echo; chmod -v 755 $(pwd)/../paquete/DEBIAN/post* -R $(pwd)/../paquete/usr/bin $(pwd)/../paquete/usr/local/bin;
  
  cp -a WifiMouseServer $(pwd)/../paquete/usr/local/bin/WifiMouseServer;
  ln -s $(pwd)/../paquete/usr/local/bin/WifiMouseServer $(pwd)/../paquete/usr/bin/WifiMouseServer;
  
  cp -a $(pwd)/../paquete/usr/share/applications/wifi-mouse-server.desktop $(pwd)/../paquete/etc/xdg/autostart/;
  
  echo "Installed-Size: $(du -ks $(pwd)/../paquete | awk '{print $1}')" >> $(pwd)/../paquete/DEBIAN/control;
  
  
  echo; dpkg-deb --build --root-owner-group $(pwd)/../paquete $(pwd)/../wifimouseserver.deb;
  echo; dpkg -i $(pwd)/../wifimouseserver.deb; echo;
