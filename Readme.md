# Çalışma şekli:

* Event kısmı ana sistemde main kısmı ise sanal makinada çalıştırılır.
* `mkfifo /dev/eta-mouse` ile fifo oluşturulur.
* Qemuya `-serial pipe:/dev/eta-mouse` parametresi eklenir.
* Qemuya usb eklemek için `-device qemu-xhci,id=xhci -device usb-host,vendorid=0x2621,productid=0x4501` yazılır.
* Qemu içerisindeki sistemde /dev/ttyS0 içine eventler yazılır. 
* event dinleyen kısım bunu dinler ve mouse olarak kullanmaya çalışır.


Sanal makinaya etap-5.3 kurulur. Ana sistemde ne olduğunun bir önemi yok.
