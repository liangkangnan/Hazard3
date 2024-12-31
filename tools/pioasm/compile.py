from adafruit_pioasm import assemble
import sys

args = sys.argv

if len(args) < 4:
  print("Usage: " + args[0] + " <text program file> <hex program file> <c header file>")
  sys.exit(1)

f = open(args[1],"r");
text = f.read();
f.close()

bin, program_name, pio_args = assemble(text)

#print('program name: ', program_name)
#print('wrap: ', pio_args['wrap'])
#print('wrap_target: ', pio_args['wrap_target'])

f = open(args[2],"w")

for i in bin:
  h = format(i,"04x")
  f.write(h + "\n")

f.close()

f = open(args[3],"w")

f.write('#ifndef _PIO_' + program_name.upper() + '_H_' + '\n')
f.write('#define _PIO_' + program_name.upper() + '_H_' + '\n\n')

f.write('#define ' + program_name + '_wrap_target ' + str(pio_args['wrap_target']))
f.write('\n')
f.write('#define ' + program_name + '_wrap        ' + str(pio_args['wrap']))
f.write('\n\n')

f.write('#define ' + program_name + '_wrap_bottom ' + program_name + '_wrap_target')
f.write('\n')
f.write('#define ' + program_name + '_wrap_top    ' + program_name + '_wrap')
f.write('\n\n')

f.write('static const uint16_t ' + program_name + '_program_instructions[] = {\n')

for i in bin:
  h = format(i,"04x")
  f.write("    0x" + h + "," + "\n")

f.write('};\n\n')

f.write('static const struct pio_program ' + program_name + '_program = {\n')
f.write('    .instructions = ' + program_name + '_program_instructions,\n')
f.write('    .length = sizeof(' + program_name + '_program_instructions)' + ' / sizeof(' + program_name + '_program_instructions[0]' + '),\n')
f.write('};\n\n')

f.write('#endif\n')

f.close()
