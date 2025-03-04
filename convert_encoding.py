import os
import sys
import chardet

def convert_to_utf8(dir_path):
    """
    지정된 디렉토리의 .cpp 및 .h 파일들을 BOM 추가 UTF-8 인코딩으로 변환합니다.

    Args:
        dir_path (str): 디렉토리 경로
    """
    if not os.path.isdir(dir_path):
        print(f"오류: 디렉토리 '{dir_path}'를 찾을 수 없습니다.")
        return

    for root, _, files in os.walk(dir_path):
        for file in files:
            if file.endswith((".cpp", ".h")):
                file_path = os.path.join(root, file)
                try:
                    convert_file_encoding(file_path)
                except Exception as e:
                    print(f"오류: 파일 '{file_path}' 변환 실패 - {e}")

def convert_file_encoding(file_path):
    """
    단일 파일의 인코딩을 BOM 추가 UTF-8로 변환합니다.

    Args:
        file_path (str): 파일 경로
    """
    try:
        # 파일 내용 읽기 (인코딩 자동 감지)
        with open(file_path, 'rb') as f:
            raw_data = f.read()
            result = chardet.detect(raw_data)
            original_encoding = result['encoding']

        if original_encoding:
            try:
                # 원래 인코딩으로 디코딩
                text = raw_data.decode(original_encoding)
            except UnicodeDecodeError:
                print(f"경고: 파일 '{file_path}' 디코딩 실패 (인코딩: {original_encoding}). 기본 인코딩으로 시도합니다.")
                text = raw_data.decode('cp949', errors='ignore') # 필요에 따라 다른 기본 인코딩 시도

            # BOM 추가 UTF-8로 인코딩하여 파일에 쓰기 (encoding='utf-8-sig' 로 변경)
            with open(file_path, 'w', encoding='utf-8-sig', newline='\n') as f: # encoding='utf-8-sig' 사용, newline='\n' 유지
                f.write(text)
            print(f"성공: 파일 '{file_path}' BOM 추가 UTF-8로 변환됨 (원래 인코딩: {original_encoding})")
        else:
            print(f"경고: 파일 '{file_path}' 인코딩 감지 실패. UTF-8 변환을 건너뜁니다.")

    except Exception as e:
        raise Exception(f"파일 처리 오류: {e}")

if __name__ == "__main__":
    if len(sys.argv) == 2:
        dir_path = sys.argv[1]
    else:
        # 디렉토리 경로가 제공되지 않은 경우 현재 스크립트 파일의 디렉토리를 기본 경로로 사용
        dir_path = os.path.dirname(os.path.abspath(__file__))

    convert_to_utf8(dir_path)
    print("인코딩 변환 완료.")