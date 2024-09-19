set rptdir [file join $outdir report]
file mkdir $rptdir
set rptutil [file join $rptdir utilization.txt]
report_datasheet -file [file join $rptdir datasheet.txt]
report_utilization -hierarchical -file $rptutil
report_clock_utilization -file $rptutil -append
report_ram_utilization -file $rptutil -append -detail
report_timing_summary -file [file join $rptdir timing.txt] -slack_lesser_than 0 -max_paths 10
report_high_fanout_nets -file [file join $rptdir fanout.txt] -timing -load_types -max_nets 25
report_drc -file [file join $rptdir drc.txt]
report_io -file [file join $rptdir io.txt]
report_clocks -file [file join $rptdir clocks.txt]
