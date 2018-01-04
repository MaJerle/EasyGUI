/**
 * \addtogroup      GUI_WINDOW
 * \{
 *
 * Core functions capable of encoding and decoding UTF-8 UNICODE format.
 *
 * Decoding of raw unicode bytes can take up to 4 bytes in a row and stores value to 32-bit variable
 *
 * \par UNICODE structure table
 *
 * <table>
 *  <tr><th>Number of bytes <th>Bits for code point <th>First code points   <th>Last code point <th>Byte 1      <th>Byte 2      <th>Byte 3      <th>Byte 4
 *  <tr><td>1               <td>7                   <td>U+0000              <td>U+007F          <td>0xxxxxxx    <td>-           <td>-           <td>-
 *  <tr><td>2               <td>11                  <td>U+0080              <td>U+07FF          <td>110xxxxx    <td>10xxxxxx    <td>-           <td>-
 *  <tr><td>3               <td>16                  <td>U+0800              <td>U+FFFF          <td>1110xxxx    <td>10xxxxxx    <td>10xxxxxx    <td>-
 *  <tr><td>4               <td>21                  <td>U+10000             <td>U+10FFFF        <td>11110xxx    <td>10xxxxxx    <td>10xxxxxx    <td>10xxxxxx
 * </table>
 *
 * https://en.wikipedia.org/wiki/UTF-8
 *
 * \}
 */