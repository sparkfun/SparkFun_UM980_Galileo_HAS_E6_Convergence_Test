Testing the convergence times on the new Galileo High Accuracy Service (HAS)
========================================

There is a new service provided by the Galileo constellation on the E6 band called High Accuracy Service ([HAS](https://www.gsc-europa.eu/galileo/services/galileo-high-accuracy-service-has)). While its convergence times are not fast, and not many receivers support the E6 band as of writing, it’s a **very** interesting ***free*** high-precision corrections service.

Tldr; If your receiver can use E6 corrections, and you’ve got some time (around 10 minutes), you can achieve 80mm precision or better, with a ‘best seen’ of 33mm, with no other corrections! Free is good. I like free.

![SparkFun Galileo HAS E6 PPP Convergence - All plots](<SparkFun Galileo HAS E6 PPP Convergence - All plots.png>)

*SparkFun Galileo HAS E6 PPP Convergence - All plots*

Above are 78 tests of the UM980 GNSS receiver outputting the latitude deviation over time. In general the lower the deviation, the better the accuracy of the solution. Note that the UM980 was reset and hot-started after every test. The output was monitored for LLA deviations over time, as well as fix type. In general, the UM980 immediately had a 3D GNSS Fix, then a PPP converging solution, followed by a PPP convergence. Once the module achieved convergence, the device was immediately reset, and the process was started again. In the graph above we can see the majority of tests (except one) finished in under 1400 seconds. 

![Start of convergence times](<SparkFun Galileo HAS E6 PPP Convergence - All plots Best Time Converging.png>)

*Start of convergence times*

Above, the start of convergence fell between 129 and 307 seconds. 

![Galileo HAS E6 PPP Average Convergence Time](<SparkFun Galileo HAS E6 PPP Convergence - Average Convergence Times.png>)

*Galileo HAS E6 PPP Average Convergence Time*

The average time to convergence was 642 seconds (including the big 4468s = ~75 minute outlier) with an average latitude deviation of 78mm. If we remove the 4468s outlier, the average convergence drops to 586 seconds (9.77 minutes) and the average latitude deviation remains the same at 77mm.

## Hardware Setup

The SparkFun [RTK Torch](https://www.sparkfun.com/products/25662) was connected to an [external tri-band antenna](https://www.sparkfun.com/products/21801) mounted on the roof with a clear view of the sky. The RTK Torch was partially disassembled to access the internal MMCX connector to attach an external antenna. The RTK Torch contains the UM980 GNSS receiver and an ESP32 microcontroller. The UM980 receiver was running firmware v11833.

## Things to change in the future

Running an additional test that allows the receiver to run for an additional 60 or 120s after PPP convergence to show how low the deviations become would be beneficial.

## Sources

* [Sketch](https://raw.githubusercontent.com/sparkfun/SparkFun_UM980_Galileo_HAS_E6_Convergence_Test/main/Galileo_HAS_Convergence_Graphing/Galileo_HAS_Convergence_Graphing.ino)
* [Convergence Times CSV](https://raw.githubusercontent.com/sparkfun/SparkFun_UM980_Galileo_HAS_E6_Convergence_Test/main/convergence_times.csv)
* [Convergence time averages](https://docs.google.com/spreadsheets/d/1PqXe-M13D_VaADYYwh_eJJQNHea8rYF7eWk6pj-U8QE/edit#gid=0) (in the order they were received)
* [SparkFun RTK Torch](https://www.sparkfun.com/products/25662)
* [SparkFun SPK6618H Triband GNSS antenna](https://www.sparkfun.com/products/21801)

## License Information

This information and this repo are ***open source***!

Please review the [`LICENSE.md`](./LICENSE.md) file for license information.

If you have any questions or concerns about licensing, please contact technical support on our [SparkFun forums](https://forum.sparkfun.com/viewforum.php?f=152).

Distributed as-is; no warranty is given.

- Your friends at SparkFun.
