
This project focuses on the EDA algorithm that deals with the physical design part of ASIC flow.

The first part is concerned with placing the blocks into two-by-four grids in order to get the most optimized cut length between the gate. Given a netlist, which consists of n logic gates, apply the minimum-cut placement such that the gates are placed in 2 times 4 layout grid ( 2 rows and 4 columns). The initial cut should be found using initial partitioning with the KL algorithm and be considered a vertical cut. The cutline directions are to be alternating. The output specifies the final placement of the gates in the layout grid. 


The second Algorithm deals with partitioning a genetic netlist into two groups following Kernighan–Lin algorithm, with a test case and output, partitioned netlist.

The third part deals with optimizing the area of the physical layer by applying shape function optimizing algorithms to get the shape and aspect possible aspects,
A test case netlist is uploaded of each block and compline them into one circuit. The output is the minimum floorplan area, aspect ratio, percentage of the wasted area (core utilization), and each block dimension. The possible dimensions for each gate are inserted in the code where each gate can be flipped 90 degrees (i.e: lengths and widths are interchangeable.
