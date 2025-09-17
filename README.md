# Proiect: PRELUCRARE GRAFICA


## 1. Prezentarea temei

Proiectul constă într-o aplicație **OpenGL** ce oferă o scenă 3D interactivă, creată pentru a demonstra tehnici moderne de grafică computerizată. Scena integrează mai multe modele 3D, inclusiv o plajă, valuri dinamice, felinare și un skybox, toate proiectate să creeze o atmosferă realistă. Utilizatorul are posibilitatea de a naviga liber prin scenă folosind o cameră controlată de la tastatură și mouse, având opțiunea de a modifica dimensiunea, poziția și rotația obiectelor.

---

## 2. Scenariu

### 2.1 Descrierea scenei și a obiectelor

Scena reprezintă o plajă cu o casă modernă, fântână și statui, palmieri, două șezlonguri, o umbrelă, o minge, un leagăn, un castel de nisip și o vedere la mare unde se observă niște delfini.

**Scena este formată din următoarele obiecte 3D:**
* 2 plane: unul pentru nisip, unul pentru mare
* casă modernă
* palmierii
* fântâna și statuile
* mingea de plajă
* umbrela de plajă
* 2 șezlonguri
* valurile din mare
* cei 3 delfini
* castelul de nisip
* leagănul
* 2 felinare

Tuturor acestor obiecte le sunt atribuite texturi corespunzătoare și realiste. Valurile din apă sunt animate pentru a oferi un efect realist al mării.

### 2.2 Funcționalități

1.  **Mișcarea camerei:** Utilizatorul poate explora scena mișcând camera în toate direcțiile: înainte, înapoi, la stânga, la dreapta, în sus și în jos, folosind tastele dedicate. Unghiul de vizualizare poate fi ajustat prin mișcarea mouse-ului, oferind o experiență flexibilă și intuitivă de navigare.
2.  **Animația valurilor:** Adaugă un nivel suplimentar de interactivitate și realism scenei.

---

## 3. Detalii de implementare

### 3.1 Funcții și algoritmi

### 3.2 Soluții posibile

Aplicația utilizează mai multe tehnici de grafică 3D:

* **Shader-e:** Acestea controlează modul în care obiectele sunt redate pe ecran, incluzând iluminarea, umbrele și texturile. S-au utilizat shader-e scrise în GLSL pentru a implementa iluminarea, umbrele și texturile. Shader-ul de iluminare folosește un model de iluminare Phong, iar efectele de umbre sunt generate folosind o hartă de umbre (shadow mapping).
* **Cameră:** Mișcarea camerei este gestionată printr-un sistem de control bazat pe taste și mișcarea mouse-ului. Sistemul se bazează pe manipularea matricelor de proiecție și vizualizare. Mișcările translaționale (față, spate, stânga, dreapta, sus, jos) sunt gestionate prin combinarea tastelor WASD și a săgeților, iar rotația camerei este controlată prin poziția mouse-ului.
* **Animații:** Animațiile sunt implementate prin interpolare liniară și sinusoidală, folosite pentru a genera mișcări naturale, precum valurile apei. S-a utilizat un timer bazat pe un shader sau pe sincronizarea cu ceasul aplicației pentru a garanta continuitatea mișcării.
* **Skybox:** Este realizat utilizând un cub map texturat, unde fiecare față a cubului reprezintă o secțiune a panoramei cerului. Texturile sunt încărcate dintr-un set de imagini HDR.

### 3.3 Motivarea abordării alese

* **Motivație Shader-e:** Această abordare permite o personalizare avansată a aspectului grafic și o performanță optimă.
* **Motivație Cameră:** Soluția oferă o navigare fluidă și intuitivă, similară cu cele mai populare sisteme de control din jocurile video.
* **Motivație Animații:** Abordarea simplă, dar eficientă, permite o simulare realistă a mișcărilor cu un impact redus asupra performanței.
* **Motivație SkyBox:** Alegerea cub map-urilor permite crearea unui fundal panoramic realist care îmbunătățește senzația de adâncime și realism fără a consuma resurse semnificative.

### 3.4 Modelul grafic

Scena este formată dintr-o serie de obiecte 3D, fiecare reprezentând un model încărcat dintr-un fișier OBJ. Modelele sunt procesate folosind shader-e pentru a le aplica texturi și efecte de iluminare.

1.  **Obiectele 3D:**
    Obiectele din scenă, cum ar fi casa, palmierii, delfinii și fântâna, sunt încărcate din fișiere de tip `.obj` sau `.fbx`. Aceste fișiere, create în Blender, conțin informații despre geometrie și materiale.
    

2.  **Camera:**
    Camera este implementată utilizând o matrice de vizualizare și o matrice de proiecție.
    
3.  **SkyBox:**
    Skybox-ul este creat folosind un cub map texturat, constând în șase imagini mapate pe fețele unui cub. Cubul este desenat în jurul camerei și este scalat suficient de mare pentru a da iluzia unui orizont infinit.
    

### 3.5 Structuri de date

Structurile de date sunt fundamentale pentru gestionarea obiectelor 3D, a camerei, a transformărilor geometrice și a interacțiunii utilizatorului cu scena.

Exemple:
* `glm::mat4` și `glm::mat3`: utilizate pentru matricile de transformare (model, vizualizare, proiecție) și pentru calculul normalMatrix.

### 3.6 Ierarhia de clase

Clasele utilizate sunt:
1.  **Window:** Creează și gestionează fereastra OpenGL.
2.  **Shader:** Gestionează încărcarea și utilizarea shader-elor.
3.  **Camera:** Controlează mișcarea camerei și vizualizarea scenei.
4.  **Model3D:** Încarcă și desenează obiecte 3D.
5.  **SkyBox:** Gestionează fundalul cerului.

---

## 4. Prezentarea interfeței grafice utilizator / manual de utilizare

**Comenzi disponibile:**
1.  **Mișcare cameră:**
    * `W` (înainte)
    * `S` (înapoi)
    * `A` (stânga)
    * `D` (dreapta)
    * `UP` (sus)
    * `DOWN` (jos)
2.  **Zoom:** Folosește rotița mouse-ului.
3.  **Rotație obiecte:**
    * `R` (rotire dreapta)
    * `F` (rotire stânga)
4.  **Moduri de vizualizare:**
    * `1` (wireframe)
    * `2` (polygonal)
    * `3` (solid)
5.  **Mărire/micșorare obiecte:**
    * `Z` (micșorare)
    * `X` (mărire)

---

## 5. Concluzii și dezvoltări ulterioare

Proiectul reprezintă o aplicație interactivă OpenGL care combină tehnici avansate de grafică 3D pentru a crea o experiență captivantă. Implementarea unei scene 3D detaliate, care include obiecte interactive, mișcarea camerei, animații și un skybox, permite utilizatorilor să exploreze și să interacționeze cu un mediu virtual complex. Abordările alese au contribuit la crearea unui sistem flexibil, ușor de extins și de personalizat.

**Pentru a extinde și îmbunătăți aplicația, se pot implementa următoarele funcții în viitor:**
* Implementarea unui sistem de selecție a obiectelor din scenă, utilizând mouse-ul, pentru a permite modificări asupra lor (ex: schimbarea culorii, rotație, scalare).
* Crearea unor animații care sunt declanșate de interacțiuni ale utilizatorului (ex: când un obiect este selectat sau mutat).
* Posibilitatea ca utilizatorul să interacționeze cu skybox-ul sau să modifice scena în funcție de mediul înconjurător (ex: schimbarea cerului sau a luminii).

---

## 6. Referințe

* OpenGL Documentation: `https://www.opengl.org/documentation/`
* GLM Documentation: `https://github.com/g-truc/glm`
* GLFW Documentation: `https://www.glfw.org/documentation/`
