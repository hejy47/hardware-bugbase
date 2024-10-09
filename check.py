from pathlib import Path
import subprocess
import toml

def check(proj_dir: Path, bug_name: str, output_file_name: str, oracle_file_name: str, exe_name: str):
  cmd = f"make SIM_SOURCES={exe_name}"
  # print(f'cmd = {cmd}')
  subprocess.run(cmd, shell=True, cwd=proj_dir, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

  subprocess.run(f'./{exe_name}_test', shell=True, cwd=proj_dir, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  or_file = proj_dir / Path(oracle_file_name)
  ot_file = proj_dir / Path(output_file_name) 
  assert ot_file.exists()
  assert or_file.exists()
  
  or_lines = or_file.read_text()
  ot_lines = ot_file.read_text()
  
  if (or_lines == ot_lines) ^ ('bug' in exe_name):
    print(f'{bug_name} SUCECSS @ {exe_name} !')
  else:
    print(f'{bug_name} FAILED  @ {exe_name} !')


def check_buggy(proj_dir: Path, bug_name: str, output_file_name: str, oracle_file_name: str):
  print(f'checking BUGGY: {cur_dir}: {bug_name} with output_file = {output_file_name}')
  check(proj_dir, bug_name, output_file_name, oracle_file_name, exe_name=f'sources_bug_{bug_name}')

def check_oracle(proj_dir: Path, bug_name: str, output_file_name: str, oracle_file_name: str):
  print(f'checking ORACLE: {cur_dir}: {bug_name} with output_file = {output_file_name}')
  ofn = 'oracle'
  if 'tb0' in oracle_file_name or 'tb1' in oracle_file_name:
    ofn = oracle_file_name[:-4]
  check(proj_dir, bug_name, output_file_name, oracle_file_name, exe_name=f'sources_{ofn}')
  

if __name__ == '__main__':
  proj_names =[
    's1', 
    's2', 's3', 'c4', 'd4', 'd12', 'd11', 'd13', 
    'd8', 
    'c1', 
    'c3', 
    'd9'
  ]
  
  for proj_name in proj_names:
    for cur_dir in Path('.').glob('*'):
      if not cur_dir.is_dir() or not cur_dir.name.startswith(proj_name): continue
      with open(cur_dir / 'project.toml') as fp:
        proj_toml = toml.load(fp)
        for bid, bug in enumerate(proj_toml['bugs']):
          for tid, tb in enumerate(proj_toml['testbenches']):
            if bug['name'] in tb['name']:
              output_file_name = proj_toml['testbenches'][tid]['output']
              oracle_file_name = proj_toml['testbenches'][tid]['oracle']
              bug_name = bug['name']
              check_buggy(cur_dir, bug_name, output_file_name, oracle_file_name)
              check_oracle(cur_dir, bug_name, output_file_name, oracle_file_name)
