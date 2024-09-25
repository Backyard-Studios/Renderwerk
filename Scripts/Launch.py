import sys
import Setup
import Clean

if len(sys.argv) < 2:
		print("No command provided")
		sys.exit(1)

command = sys.argv[1]

if command == 'setup':
	Setup.Execute()
elif command == 'clean':
	Clean.Execute()

