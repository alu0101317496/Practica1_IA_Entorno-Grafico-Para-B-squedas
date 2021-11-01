# **Uso de programa Taxi Autónomo**

## 1. **Ejecución:**
Para ejecutar el programa, se arrastrará el archivo .html en el navegador web de preferencia. Conversamente, darle doble click lo abrirá en el navegador predeterminado de igual forma.

## 2. **Uso:**
* ### Creación de malla:
    + Para crear una malla clickaremos en el botón "Apply", que saldrá acorde a los parámetros predeterminados.

    + Si queremos cambiar estos parámetros, basta con hacer click y teclear los nuevos valores en las casillas correspondientes. (Altura, Anchura y Porcentaje de obstáculos) Volvemos a clickar "Apply"

    + Si queremos introducir/quitar obstáculos, hacemos click derecho directamente en el canvas, en la posición que queremos cambiar a su opuesto.

    + Para vaciar la malla de todo obstáculo, clickamos en el botón "Clear"

    + También podemos introducir obstáculos desde un fichero de texto, introducido al seleccionar archivo en "Lectura de obstáculos" (Solo después de haber creado la malla con "Apply")

        - Se tomarán como posiciones cualquier número contenido en el fichero, este se leerá de izquierda a derecha y de arriba a abajo.

        - Cada dos cadenas numéricas se crea un nuevo obstáculo en la malla, con la posición X e Y igual a los valores de las cadenas.

        - Si la posición no existe en la malla, simplemente se desecha.

    * ### Creación de camino:
        + Para asignar las posiciones de inicio y final, hacemos click izquierdo en dos posiciones sin obstáculos del canvas; el camino se creará con los parámetros predeterminados.

        + Si queremos cambiar esos parámetros, basta con hacer click y en la casilla correspondiente y seleccionar los nuevos valores (Heurística, Tipo de movimiento)

        + Una vez se cambien los parámetros, seleccionar de nuevo los puntos de inicio y final para recrear el camino óptimo.

        + Si se selecciona como punto de inicio/final una casilla con obstáculo; o si no existe camino entre los dos puntos, saltará una advertencia.

