# Building My Own Operating System (C++ & Assembly)

*Türkçe versiyonu aşağıdadır / Turkish version is below.*

This repository contains the source code and my personal dev log for a custom OS kernel I'm building from scratch.

### Chapters 1 & 2: Booting & Tooling

* Wrote a basic assembly bootloader (`loader.s`) and a minimal C++ kernel to print text directly to the VGA buffer.
* Got tired of generating manual ISOs, so I wrote a `Makefile` and integrated QEMU. Now, a simple `make run` cleans, compiles, builds the ISO, and spins up the VM instantly.

### Chapter 3: Global Descriptor Table (GDT)

* Implemented the GDT to define physical memory boundaries and privileges. It sets the hard lines between "kernel space" and "user space." You can't safely handle hardware interrupts without this foundation.

### Chapter 4: I/O Ports & Screen Management

* Wrapped the ugly low-level Assembly `inb` and `outb` instructions into clean C++ classes (`port.h`, `port.cpp`).
* Built a custom `printf` implementation that tracks the cursor, handles newlines (`\n`), and clears the screen automatically when the terminal fills up.

### Chapter 5: Interrupts & IDT

* Set up the Interrupt Descriptor Table (IDT) and programmed the PIC to route hardware signals.
* Since interrupts can hit at any time, I wrote Assembly stubs (`interruptstubs.s`) to safely push CPU registers to the stack before handing the signal to C++, preventing total system crashes.

### Chapter 6: Keyboard Driver

* Fixed a major lockup issue: the CPU was freezing because it didn't send an "Acknowledge" (ACK) signal back to the PIC.
* Started polling Port 0x60 and 0x64, mapping raw hardware scancodes to actual ASCII characters.

### Chapter 7: Mouse Driver & The PS/2 Collision

* Wrote a mouse driver that reads 3-byte packets to render a moving cursor via bitwise VGA color shifting.
* **Bug Fix:** Hit a nasty hardware quirk where moving the mouse permanently locked up the keyboard. Both share the PS/2 controller, and the keyboard was leaving a raw `0xFA` (ACK) byte on the bus on startup. The mouse read this junk data, corrupting the bus. Fixed it by doing a dummy read during keyboard initialization to flush the pipeline.

### Chapter 8: Driver Abstractions & Event Handlers

* Ripped out all the hardcoded `printf` calls from the drivers. Hardware shouldn't care about rendering text.
* Built an event-driven architecture (`DriverManager`, `KeyboardEventHandler`). Now, the driver just broadcasts a "key pressed" event, and the kernel handles the UI implementation. This makes the OS ready for a real GUI later.

### Chapters 10 & 11: PCI (Motherboard Scanning) & BAR (Hardware Communication)

**The Old Way (Why did we change it?)**
* In previous chapters, we hardcoded the keyboard and mouse addresses directly into the code (e.g., "The keyboard is definitely at port 0x60"). This is like saying, "My friend definitely lives at house number 5." However, complex parts like graphics cards or network adapters are plugged into different slots on every computer. We can't memorize their addresses in advance.

**1. The Upgrade: PCI (The Motherboard's Receptionist)**
* Thanks to the **PCI (Peripheral Component Interconnect)** system we wrote, our OS stopped blindly guessing addresses. Now, it scans the entire motherboard and asks, "Who is in slot 1? Who is in slot 2?". The PCI acts like a receptionist, listing the identities of the hardware (Vendor ID and Device ID) by saying, "There is an Intel GPU here, and an AMD Network Card there." We finally upgraded the system to a true "Plug-and-Play" architecture.

**2. The Upgrade: BAR (The Direct Phone Number)**
* Finding the graphics card isn't enough; how do we send pixels and colors to it? This is where the **BAR (Base Address Register)** comes into play. The BAR is the direct communication box of that hardware. By reading the BAR value, our OS can figure out exactly which port address to use to talk to the device.

**In summary:** We taught the system to automatically ask the hardware, *"Who are you, and at what number can I communicate with you?"*
=======
### Chapters 10 & 11: PCI & Base Address Registers (BARs)

* Wrote a PCI controller to scan the motherboard's buses, devices, and functions.
* The OS can now dynamically discover plugged-in hardware (like AMD network chips or VGA graphics cards) using their Vendor and Device IDs.
* By reading the Base Address Registers (BARs), the kernel automatically figures out which I/O ports or memory maps each device uses. I no longer have to hardcode hardware ports!)

Chapters 12 & 13: Graphics Mode (VGA) & GUI Framework Basics
Why did we need this?

Up until now, we could only print plain white text on a black screen. To build a modern operating system with windows, a moving mouse cursor, and clickable buttons, we had to unlock the graphics mode.

1. The VGA Graphics Driver

Since we are running in 32-bit protected mode, we can't cheat by using the easy BIOS interrupts to switch screen resolutions. Instead, we wrote a driver that communicates directly with the graphics card's hardware ports to force the system into a 320x200 graphics mode. Now, instead of just printing ASCII characters, we can draw individual pixels of any color!

2. The Object-Oriented GUI Framework

Using our new pixel-drawing superpower, we laid the foundation for a real graphical user interface. We created a base Widget class and built a Desktop and Window system on top of it. The OS now has a spatial awareness system: it can track mouse coordinates, calculate exactly which window or button you are hovering over, and pass click events to the right object.
---

# Kendi İşletim Sistemimi Yazıyorum (C++ & Assembly)

Bu repo, sıfırdan geliştirdiğim işletim sistemi çekirdeğinin (kernel) kaynak kodlarını ve kişisel geliştirme günlüğümü (dev log) içeriyor.

### Bölüm 1 ve 2: Bootloader & Test Otomasyonu

* Sistemi ayağa kaldırmak için `loader.s` ile küçük bir assembly başlatıcısı yazdım ve VGA belleğine doğrudan müdahale ederek ekrana ilk çıktıyı aldım.
* Sürekli manuel ISO oluşturma hamallığından kurtulmak için projeye Makefile ve QEMU entegre ettim. Artık tek bir `make run` komutuyla her şey derlenip sanal makine saniyeler içinde açılıyor.

### Bölüm 3: Global Descriptor Table (GDT)

* İşlemciye bellek sınırlarını ve erişim yetkilerini (kernel vs. user space) tanımlamak için GDT'yi kurdum. Bu harita olmadan donanımdan gelen anlık sinyalleri (interrupts) sistemi çökertmeden yakalamak mimari olarak imkansızdı.

### Bölüm 4: I/O Portları & Ekran Yönetimi

* İşlemcinin disk, klavye gibi donanımlarla konuşmasını sağlayan Assembly `inb` ve `outb` komutlarını nesne yönelimli C++ sınıfları (OOP) haline getirdim.
* Kendi `printf` fonksiyonumu yazarak alt satıra geçebilen (`\n`) ve ekran dolduğunda sayfayı temizleyen bir imleç (cursor) mantığı kurdum.

### Bölüm 5: IDT ve Kesmeler (Interrupts)

* Donanım sinyallerini yakalamak için IDT haritasını çıkardım ve sinyal trafiğini yönetsin diye PIC çipini programladım.
* C++ doğrudan interrupt yakalarsa sistem patlayacağı için, araya Assembly (`interruptstubs.s`) ile CPU register'larını yedeğe alıp geri yükleyen (push/pop) köprü kodlar yazdım.

### Bölüm 6: Klavye Sürücüsü

* Sistem donanım sinyali aldığında PIC'e "ACK" (onay) göndermediği için kilitleniyordu, bunu çözdüm.
* 0x60 portu üzerinden klavyeyi dinleyip gelen donanımsal tuş kodlarını (scancodes) ASCII harflere çeviren ilk gerçek sürücüyü tamamladım.

### Bölüm 7: Fare Sürücüsü ve PS/2 Çakışması

* Fareden gelen 3 byte'lık veri paketlerini okuyup ekrandaki piksellerin renklerini ters çevirerek hareket eden bir imleç render ettim.
* **Bug Fix:** Fareyi hareket ettirince klavyenin kilitlendiği iğrenç bir donanım bug'ıyla boğuştum. Klavye uyanırken ortak PS/2 veri yolunda `0xFA` sinyali bırakıyordu ve fare bunu kendi konfigürasyonu sanıp sistemi kilitliyordu. Klavye constructor'ına tek satırlık bir "dummy read" (boş okuma) ekleyerek veri yolunu temizledim ve sorunu çözdüm.

### Bölüm 8: Sürücü Soyutlaması (Abstraction)

* Sürücülerin içindeki amele `printf` kodlarını tamamen sildim; donanımın görevi ekrana yazı yazmak değildir.
* Olay tabanlı (event-driven) bir mimari kurdum. Artık klavye sürücüsü sadece "tuşa basıldı" eventi fırlatıyor, bu veriyi ekranda gösterme işini Kernel üstleniyor. Bu hamle sistemi ileride pencereli bir GUI'a geçmeye hazır hale getirdi.

<<<<<<< HEAD
### Klavyedeki Tuşları Nasıl Okuyoruz?
* Klavye ile iletişim için Port 0x60 (veri) ve Port 0x64 (komut) kapılarını dinliyoruz. Klavyeden bir tuşa basıldığında bize harfin kendisi değil, donanımsal bi sayı gelir. Sürücümüzün içine bir çevirici yazarak bu sayıları harflere dönüştürdük. Böylece işletim sistemimizin ilk gerçek donanım sürücüsünü yazmış olduk.


## Bölüm 7: Klavye ve Fare Sürücüleri (Donanımla Etkileşim)

### Fare (Mouse) Sürücüsünün Mantığı Nedir?
* Fare, her hareket ettiğinde veya tıklandığında işlemciye 3 parça (byte) veri yollar: Tıklama durumu, X ekseni hareketi ve Y ekseni hareketi.
* Bu üç veriyi bir tamponda (buffer) toplayarak okuduk ve farenin bulunduğu kordinattaki ekran bellek piksellerinin (VGA) arka plan/ön plan renklerini ters çevirerek (bitwise shift işlemleriyle) ekranda görsel bir imleç oluşturduk.

## Bölüm 8: Sürücü Soyutlaması (Driver Abstraction) ve Olay Yöneticileri

### Neden Sürücüleri Soyutladık? (Abstraction)
* Önceki bölümlerde klavye ve fare sürücülerimiz doğrudan ekran belleğine (VGA) müdahale edip `printf` ile pikselleri boyuyordu. Bu yapı, ileride siyah terminal ekranından çıkıp pencereli bir Grafik Arayüzüne (GUI) geçtiğimizde büyük bir tasarım karmaşasına yol açacaktı. Donanımın görevi ekrana yazı yazmak değil, sadece donanımsal veriyi (örneğin "tuşa basıldı") işletim sistemine iletmektir.

### Ne Yaptık? (OOP Mimarisinin Gücü)
* **`Driver` ve `DriverManager` Sınıfları:** Tüm donanım sürücülerinin türeyeceği ortak bir ata (base) sınıf oluşturduk. `DriverManager` sayesinde sistemdeki klavye, fare (ve ileride eklenecek ağ kartı vb.) tüm sürücüleri tek bir liste içine alıp, hepsini tek satırda (`ActivateAll`) başlatabilir hale geldik.
* **Olay Yöneticileri (Event Handlers):** Donanım (sürücü) ile yazılım (çekirdek) arasına `KeyboardEventHandler` ve `MouseEventHandler` adında bağımsız köprüler kurduk.
* Artık klavyeden 'A' tuşuna basıldığında klavye sürücüsü ekrana 'A' *yazdırmıyor*. Bunun yerine çekirdeğe (kernel) "A tuşuna basıldı" olayı (event) fırlatıyor. Çekirdek (`kernel.cpp`) kendi içindeki özel sınıflarla (`PrintfKeyboardEventHandler`, `MouseToConsole`) bu olayı yakalayıp ekranda gösteriyor.

### Sonuç
Sistemimiz tamamen **Modüler** hale geldi. Yarın grafik arayüze (GUI) geçtiğimizde sürücü (`.cpp`) kodlarımıza hiç dokunmadan, sadece Kernel tarafındaki yöneticileri değiştirerek işletim sistemimizi güncelleyebileceğiz.
### Karşılaştığımız Kritik Hata: PS/2 Veri Hattı Çakışması
* Fare imleci hareket ederken klavye tamamen kilitlendi ve tepki vermedi.Klavye ve fare anakart üzerindeki aynı PS/2 kontrolcüsünü paylaşır. Klavye uyanma komutu aldığında hatta `0xFA` (uyandım) onay mesajı bırakır. Biz bu mesajı okuyup hattı temizlemeden fare sürücüsünü başlattığımız için, fare bu mesajı kendi donanım ayarı zannedip okuyor ve klavyenin kesme sinyallerini kilitliyordu.
* Klavye sürücüsünün yapıcı fonksiyonunun (constructor) en sonuna fazladan bir `dataport.Read()` ekleyerek hattı temizledik (onay mesajını yuttuk) ve iki donanımın aynı anda kusursuz çalışmasını sağladık.
=======
## Bölüm 10 ve 11: PCI (Anakart Taraması) ve BAR (Donanımla İletişim)

### Eskiden Ne Yapıyorduk? (Neden Değiştirdik?)
* Eski bölümlerde klavye ve fare sürücülerini yazarken adreslerini kodun içine elle (hardcode) girmiştik (Örn: "Klavye kesin 0x60 portundadır"). Bu, "Arkadaşım kesin 5 numaralı evde oturuyor" demek gibidir. Ancak ekran kartı veya ağ kartı gibi parçalar her bilgisayarda farklı yuvalara takılır, adreslerini önceden ezbere bilemeyiz.

### 1. Yenilik: PCI (Anakartın Resepsiyonisti)
* Yazdığımız **PCI (Peripheral Component Interconnect)** sistemi sayesinde işletim sistemimiz körü körüne adres denemeyi bıraktı. Artık anakartı baştan sona tarayıp "1. yuvada kim var? 2. yuvada kim var?" diye sorabiliyor. PCI bize "Burada Intel Ekran Kartı var, şurada AMD Ağ Kartı var" diyerek donanımların kimliklerini (Vendor ID ve Device ID) listeliyor. Sistemi "Tak-Çalıştır" (Plug-and-Play) mantığına geçirdik.

### 2. Yenilik: BAR (Doğrudan İletişim Kutusu)
* Ekran kartını bulmak yetmez, ona pikselleri ve renkleri nasıl göndereceğiz? İşte burada **BAR (Base Address Register)** devreye giriyor. BAR, o donanımın iletişim kutusudur. İşletim sistemimiz cihazın BAR değerini okuyarak, "Ekran kartıyla haberleşmek istiyorsam verileri şu port adresine göndermeliyim" diyebiliyor.

**Özetle:** Sisteme donanımlara "Sen kimsin ve seninle hangi numaradan haberleşebilirim?" diye otomatik olarak sormayı öğrettik!
=======
### Bölüm 10 ve 11: PCI ve BAR (Base Address Registers)

* Anakarttaki PCI veri yolunu baştan sona tarayarak ekran kartı, ağ kartı gibi cihazları (Vendor ID / Device ID) dinamik olarak bulan bir PCI controller yazdım.
* Donanımların Base Address Register'larını (BAR) okuyarak hangi I/O portlarından haberleştiklerini sistemin kendisinin bulmasını sağladım. Artık hiçbir donanım portunu kodun içine hardcode yazmak zorunda değilim!

Bölüm 12 ve 13: Grafik Modu (VGA) ve Arayüz (GUI) Temelleri
Buna Neden İhtiyacımız Vardı?

Bugüne kadar sadece siyah ekrana beyaz harfler yazabiliyorduk. Gerçek bir işletim sistemi gibi pencereler, fare imleci ve tıklanabilir butonlar yapabilmek için grafik moduna geçmemiz şarttı.

1. VGA Grafik Sürücüsü

32-bit korumalı modda (protected mode) çalıştığımız için, ekran çözünürlüğünü değiştiren kolay BIOS komutlarını kullanamayız. Bunun yerine, ekran kartının donanım portlarına doğrudan karmaşık sinyaller göndererek sistemi 320x200 çözünürlüklü grafik moduna zorla geçiren bir sürücü yazdık. Artık ekrana sadece harf değil, istediğimiz renkte pikseller (noktalar) çizebiliyoruz!

2. Nesne Yönelimli Arayüz (GUI) Altyapısı

Ekrana piksel çizebilme yeteneğimizi kullanarak gerçek bir arayüzün temellerini attık. Bir Widget (Arayüz Elemanı) sınıfı oluşturduk ve bunun üzerine Masaüstü (Desktop) ile Pencereleri (Windows) inşa ettik. İşletim sistemimiz artık uzaysal bir farkındalığa sahip: Farenin koordinatlarını takip edebiliyor, ekranda tam olarak hangi pencerenin veya butonun üzerinde gezdiğinizi hesaplayabiliyor ve tıklama işlemlerini (click events) doğru donanıma iletebiliyor.
