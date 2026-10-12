# Kendi İşletim Sistemimi Yazıyorum (C++ & Assembly)

Bu depo, sıfırdan bir işletim sistemi çekirdeği (kernel) geliştirme sürecimdeki kodlarımı ve kişisel notlarımı içermektedir.

## Bölüm 1 ve 2: Çekirdek Temelleri, Ekrana Yazı Yazdırma ve Test Otomasyonu

### Ne Yaptık?
* Bilgisayarın doğrudan bizim kodlarımızı tanıması için bir başlatıcı (`loader.s`) ve temel bir C++ çekirdeği yazdık. Bellekteki ekran kartı (VGA) bölgesine müdahale ederek ekrana ilk metnimizi yazdırdık.

### Test Sürecindeki Hamallıktan Nasıl Kurtulduk?
* Her seferinde manuel ISO oluşturmak yerine `Makefile` yazdık. Ağır sanal makineler yerine daha hızlı olan **QEMU**'yu projeye entegre ettik. Artık terminale sadece `make run` yazdığımızda sistem eski dosyaları siliyor, kodları derliyor, boot edilebilir bir ISO kalıbı üretiyor ve hızlıca çalıştırıyor.

## Bölüm 3: Global Descriptor Table (GDT)

### GDT Nedir ve Neden Bu Kadar Karmaşık Bir Bellek Haritası Yazdık?
* GDT, işlemciye belleğin fiziksel sınırlarını ve erişim haklarını anlatan bir haritadır. İşlemciye "Burası işletim sisteminin tam yetkili alanı, burasıysa diğer programların alanı" dememizi sağlar. Bu harita olmadan dışarıdan gelen donanım sinyallerini güvenle yakalayıp işleyebilmemiz mimari olarak imkansızdır.

## Bölüm 4: Portlar ve Ekran Yönetimi

### Port (Bağlantı Noktası) Ne İşe Yarar?
* İşlemcinin diğer donanımlarla (klavye, fare, disk vb.) veri alışverişi yaptığı sanal kapılardır. Normalde Assembly ile yazılan karmaşık `outb` ve `inb` komutlarını C++ sınıfları (OOP) içine hapsederek, donanımlarla nesne yönelimli ve çok daha kolay iletişim kurabileceğimiz bir altyapı (`port.h` ve `port.cpp`) hazırladık.

### Printf Yani Ekranı Nasıl Geliştirdik?
* Bir imleç mantığı kurduk. Artık `\n` ile alt satıra geçebiliyor, yazılar sayfanın dışına taştığında ekranı otomatik olarak temizleyip en baştan başlayabiliyoruz.

## Bölüm 5: Donanım Kesmeleri (Interrupts) ve IDT

### Kesme (Interrupt) Nedir ve IDT (Interrupt Descriptor Table) Ne İşe Yarar?
* Kesme, klavye veya saat gibi donanımların işlemciye "Şu anki işini durdur ve bana bak!" deme şeklidir. IDT ise işlemciye bu sinyaller geldiğinde tam olarak hangi kodu çalıştırması gerektiğini gösteren haritadır. Tıpkı GDT'nin bellek sınırlarını çizmesi gibi, IDT de donanım sinyallerinin yollarını çizer.

### Neden C++ İle Doğrudan Sinyal Yakalamak Yerine Araya Assembly (`interruptstubs.s`) Kodları Ekledik?
* Dışarıdan aniden bir sinyal geldiğinde, işlemci o an başka bir programı çalıştırıyor olabilir. Mevcut sistemin çökmemesi için, Assembly ile işlemcinin o anki durumunu (register'ları) hafızaya güvenle kaydediyoruz (push). Ardından C++ fonksiyonumuzu (handler) çalıştırıp, işimiz bitince işlemcinin eski durumunu geri yüklüyoruz (pop).

### PIC (Programmable Interrupt Controller) Çipi Ne İşe Yarar?
* Donanımlar ile işlemci arasındaki trafik polisidir. İşlemcinin kendi sistem hataları ile dışarıdan gelen donanım sinyallerinin numaraları birbirine karışmasın diye, donanım sinyallerine matematiksel bir kaydırma (offset) uygular ve onları işlemciye sırayla iletir.

## BÖlüm 6: Klavye Sürücüsü (Keyboard Driver)

### İşlemci Sinyal Aldığında Neden Donuyordu ve Nasıl Çözdük?
* İşlemci bir donanımdan sinyal aldığında, PIC çipine "Mesajı aldım, sıradaki işleme geçebilirsin" demek zorundadır. Bunu demezsek sistem yeni sinyal kabul etmez ve kilitlenir. 'interrupts.cpp' içinde PIC'e cevap verme mekanizmasını kuracak bu sorunu çözdük.

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

