import json, time

def create_json_dump(f_name, ip, function_name, line_number):
    info = {
                "time_stamp": f_name,
                "instruction_pointer": ip,
                "function_name": function_name,
                "line_number": line_number
            }
    with open(f'json_crashes/{f_name}.json', "w") as f:
        f.write(json.dumps(info))

def crash_handler(event):
    print("Crash detected, dumping...")
    gdb.execute("gcore crashes/crash.dump")
    create_json_dump(str(int(time.time())), hex(gdb.selected_frame().pc()), str(gdb.selected_frame().function()), str(gdb.selected_frame().function().line))


gdb.events.stop.connect(crash_handler)
gdb.execute("r crashes/fuzzer_output.bin")
