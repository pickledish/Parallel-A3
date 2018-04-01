import subprocess
import numpy

writer = open('pythonoutput.txt', 'w+')

for name in ["histogram", "histo_private", "histo_lockfree", "histo_lock1", "histo_lock2"]:

	writer.write(r"\begin{center}" + "\n")
	writer.write(r"\begin{tabular}{ |m{5em}|m{5em}|m{5em}|m{5em}|m{5em}|m{5em}|m{5em}|m{5em}| }" + "\n")
	writer.write(r"\hline" + "\n")
	writer.write(r"\textbf{Threads} & \textbf{Hawkes} & \textbf{Earth} & \textbf{Flood} & \textbf{Noise} & \textbf{Phobos} & \textbf{Bear} & \textbf{Univ} \\ \hline" + "\n")

	for i in [1,2,4,8]:

		if (name == "histogram" and i > 1): continue

		writer.write(r"\textbf{" + str(i) + r"}")

		for nput in ["300px-Unequalized_Hawkes_Bay_NZ.ppm", "earth.ppm", "flood.ppm", "noise_8bit.ppm", "phobos.ppm", "polar_bear_in_snow_8bit.ppm", "university.ppm"]:

			runValues = []

			for _ in range(5):

				NPUT = "inputs/" + nput

				outFile = "outFile_" + name + "_" +  "_" + str(i) + "_" + nput
				command = "./" + name + " " + NPUT + " " + " " + outFile + " " + str(i)

				process = subprocess.Popen([command], stdout = subprocess.PIPE, shell = True)
				out, err = process.communicate()

				nsTime = out.split(" ")[1]

				runValues.append(int(nsTime))

			mean = str(numpy.mean(runValues))
			std = str(numpy.std(runValues))
			writer.write(r"& \makecell{" + mean + r" \\[-0.5em] " + std + r"}")

		writer.write(r"\\ \hline" + "\n")

	writer.write(r"\end{tabular} ")
	writer.write(r"\end{center} " + "\n")
	writer.write("\n\n")

writer.close()