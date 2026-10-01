# Kendi İşletim Sistemimi Yazıyorum (C++ & Assembly)
Bu depo, sıfırdan bir işletim sistemi çekirdeği(kernel) geliştirme sürecimdeki kodlarımı ve kişisel notlarımı içermektedir.

### BÖLÜM 1 VE 2 : ÇEKİRDEK TEMELLERİ,EKRANA YAZI YAZDIRMA VE TEST OTOMASYONU ###

# Ne Yaptık ?
- Bilgisarayarın doğrudan bizim kodlarımızı tanıması için bir başlatısı ('loader.s') ve temel bir C++  çekirdeği yazdık. Bellekteki ekran kartı(VGA) bölgesine müdahele ederek ekrana ilk metnimizi yazdırdık.

# Test Sürecindeki Hamallıktan Nasıl Kurtulduk?
- Her seferinde manuel ISO oluşturmak yerine 'Makefile' yazdık. Ağır sanal makineler yerine daha hızlı olan **QEMU** 'yu projeye entegre ettik. Artık terminale sadece 'make run' yazdığımızda sistem eski dosyaları siliyor,kodları derliyor,boot edilebilir bir ISO kalıbı üretiyor ve hızlıca çalıştırıyor.

### BÖLÜM 3 : GLOBAL DESCRIPTOR TABLE (GDT) ###

# GDT Nedir? Neden Bu Kadar Karmaşık Bir Bellek Haritası Yazdık?
- GDT, işlemciye belleğin fiziksel sınırlarını ve erişim haklarını anlatan bir haritadır. İşlemciye "Burası işletim sisteminin tam yetkili alanı, burasıysa diğer programların alanı." dememizi sağlar. Bu harita olmadan dışarıdan gelen donanım sinyallerini güvenle yakalayıp işleyebilmemiz mimari olarak imkansızdır.

### BÖLÜM 4: PORTLAR VE EKRAN YÖNETİMİ ##

# Port (bağlantı noktası) Ne İşe Yarar ?
- İşlemcinin diğer donanımlarla(klavye,fare,disk vb.) veri alışverişi yaptığı sanal kapılardır. Normalde Assembly ile yazılan karmaşık 'outb' ve 'inb' komutlarını C++ sınıfları (OOP) içine hapsederek, donanımlarla nesne yönelimli ve çok daha kolay iletişim kurabileceğimiz bir altyapı ('port.h' | 'port.cpp') hazırladık.

#Printf Yani Ekranı Nasıl Geliştirdik?
- Bir imleç mantığı kurduk. Artık '\n' ile alt satıra geçebiliyor, yazılar sayfanın dışına taştığında ekranı otomatik olarak temizleyip en baştan başlayabiliyoruz.
