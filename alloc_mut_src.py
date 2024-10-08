from pathlib import Path
import shutil

def find_proj_dir(name: str) -> Path:
  for dir in Path('./').glob("*"):
    if not dir.is_dir(): continue
    if dir.name.startswith(name):
      return dir
  raise NameError(f"{name} does not exist.")

def extract_bid(name: str) -> str:
  return name.split('-')[-1]

def find_buggy_src_file(dir: Path) -> str:
  for f in dir.glob('*.v'):
    if f.name.startswith('buggy'):
      return f
  raise NameError(f"dir {dir} does not exist a buggy file.")

def replace_line_in_file(file_path, old_str, new_str, output_file_path):
    # Open the file and read its lines
    with open(file_path, 'r') as file:
        lines = file.readlines()

    # Replace the entire line that contains old_str with new_str ignore ".v"
    updated_lines = ['../' + new_str + '\n' if 'rtl/' + old_str[:-2] in line else line for line in lines]

    # Write the updated lines to a new file
    with open(output_file_path, 'w') as output_file:
        output_file.writelines(updated_lines)

def process(mut_dir: Path, type: str):
  for proj in mut_dir.glob('*'):
    if not proj.is_dir(): continue

    if (proj / 'wk_dir').exists():
      shutil.rmtree(proj / 'wk_dir')
    
    for bug_name in proj.glob('*'):
      if not bug_name.is_dir(): continue
      bid = extract_bid(proj.name)
      
      if bid == 's1': 
        # ignore s1
        print('ignore s1')
        continue
      
      proj_bugbase_dir = find_proj_dir(bid)
      buggy_src_file = find_buggy_src_file(bug_name)
      oracle_src_file = '_'.join(buggy_src_file.name.split('_')[1:])
      # replace oracle source file -> buggy_src_file
      # create a file sources_mut_{bid}.txt
      replace_line_in_file(
        proj_bugbase_dir / 'sources_oracle.txt', 
        oracle_src_file, 
        str(buggy_src_file),
        proj_bugbase_dir / f'sources_mut_{type}_{bug_name.name}.txt'  
      )
      print(f'proj:{proj.name} @ bid:{bug_name.name} finished')
      break
      

if __name__ == '__main__':
  multi_path = Path('./fpga-debugging-mutation-multi')
  single_path = Path('./fpga-debugging-mutation-single')
  process(multi_path, multi_path.name.split('-')[-1])
  process(single_path, single_path.name.split('-')[-1])
