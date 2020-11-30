windowMEF <- function( filename, uUTC_start, uUTC_stop, info, password_key ) {
  #' @export
  #' 
  # data = windowMEF( macro_filename, uUTC_start, uUTC_stop, info )
  # MRB 2020 OCT
  uUTC_start_original = uUTC_start;
  uUTC_stop_original = uUTC_stop;
  
  win_d = list();
  win_t = list();
  
  discontinuities = read_mef_discont(filename,);
  SF = header.sampling_frequency;
  
  tmp1 = find( TOC[1,] <= uUTC_start );
  if ( isempty(tmp1) ) {
    return;
  }
  start_block = tmp1(end);
  start_block_index = TOC(3,start_block);
  
  tmp2 = find( TOC[1,] <= uUTC_stop );
  if ( isempty(tmp2 ) ) {
    return;
  }
  stop_block = tmp2(end);
  stop_block_index = TOC(3,stop_block);
  
  # Is there a discontinuity flag anywhere from (start_block+1) to (stop_block)?
  discExists = which( discontinuity_flags( (start_block+1) : stop_block ) );
  if (discExists) {
    blocks = (start_block+1):stop_block;
    discBlock = blocks(discExists(1));
    stop_block = discBlock - 1;
    stop_block_index = TOC(3,discBlock) - 1;
    discExists = find( discontinuity_flags( (start_block+1) : stop_block ) );
    if (discExists) {
      disp('Discontinuity in requested window. Returning empty data.');
      return;
    }
  }
  
  start_rem = uUTC_start - TOC(1,start_block);
  rem_idx = round( SF * start_rem / 1E6 );
  start_idx =  rem_idx + start_block_index;
  start_time = TOC(1,start_block) + 1E6*rem_idx/SF;
  
  stop_rem = uUTC_stop - TOC(1,stop_block);
  rem_idx = round( SF * stop_rem / 1E6 );
  stop_idx = rem_idx + stop_block_index;
  stop_time = TOC(1,stop_block) + 1E6*rem_idx/SF;
  
  if (start_idx>=stop_idx) {
    fprintf('index problem: start: %d %d\t stop: %d %d\n', uUTC_start_original, start_idx, uUTC_stop_original, stop_idx );
    return;
    end
    win_d_ = decomp_mef( macro_filename, start_idx, stop_idx, 'erlichda' );
    win_d = double( win_d_ );
    N = size( win_d, 1 );
    dt = ( stop_time - start_time ) / (N-1);
    win_t = round( ( start_time : dt : stop_time ) );
  }
}
