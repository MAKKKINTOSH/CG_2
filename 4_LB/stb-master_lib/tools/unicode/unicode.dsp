hoosing a length that's a multiple?
      int i,pos, do_split=0;
      numlen = 0;
      for (i=0; i < length; ++i) {
         if (count == 2)
            sprintf(temp, "{%d,%d}", d ? data[i] : (trims[i]>>8), trims[i]&255);
         else
            sprintf(temp, "{%d,%d,%d}", data[i], trims[i]>>8, trims[i]&255);
         len = strlen(temp);
         numlen = stb_max(len, numlen);
      }
   
      at_newline = 1;
      for (i=0; i < length; ++i) {
         if (pos + numlen + 2 > linelen) {
            printf("\n");
            at_newline = 1;
            pos = 0;
         }
         if (at_newline) {
            printf("  ");
            pos = 2;
            at_newline = 0;
         } else {
            printf(" ");
            ++pos;
         }
         if (count == 2)
            sprintf(temp, "{%d,%d}", d ? data[i] : (trims[i]>>8), trims[i]&255);
         else
            sprintf(temp, "{%d,%d,%d}", data[i], trims[i]>>8, trims[i]&255);
         printf("%*s,", numlen, temp);
         pos += numlen+1;
      }
      if (!at_newline) printf("\n");
      printf("};\n");
   }
}

int weight=1;

table pack_for_mode(table *t, int mode, char *table_name)
{
   size_t extra_size;
   int i;
   uval maxv;
   mode_info mi = modes[mode % MODECOUNT];
   int size = 8 << (mode / MODECOUNT);
   table newtab;
   uval *packed = NULL;
   uval *index = NULL;
   uval *indirect = NULL;
   uval *specials = NULL;
   newtab.dont_care = UVAL_DONT_CARE_DEFAULT;
   if (table_name)
      printf("// clusters of %d\n", size);
   for (i=0; i < t->length; i += size) {
      uval newval;
      int fastpath = (i < t->fastpath);
      if (mi.special) {
         int end_trim = size-1;
         int start_trim = 0;
         uval special;
         // @TODO: pick special from start or end instead of only end depending on which is longer
         for(;;) {
            special = t->input[i + end_trim];
            if (special != t->dont_care || end_trim == 0)
               break;
            --end_trim;
         }
         // at this point, special==inp[end_trim], and end_trim >= 0
         if (special == t->dont_care && !fastpath) {
            // entire block is don't care, so OUTPUT don't care
            stb_arr_push(index, newtab.dont_care);
            continue;
         } else {
            uval pos, trim;
            if (mi.trim_end && !fastpath) {
               while (end_trim >= 0) {
                  if (t->input[i + end_trim] == special || t->input[i + end_trim] == t->dont_care)
                     --end_trim;
                  else
                     break;
               }
            }

            if (mi.trim_start && !fastpath) {
               while (start_trim < end_trim) {
                  if (t->input[i + start_trim] == special || t->input[i + start_trim] == t->dont_care)
                     ++start_trim;
                  else
                     break;
               }
            }

            // end_trim points to the last character we have to output

            // find the first match, or add it
            pos = find_packed(&packed, &t->input[i+start_trim], end_trim-start_trim+1, mi.aligned, fastpath, &t->input[t->length], i+start_trim, t->replace_fallback_with_codepoint);

            // encode as a uval
            if (!mi.trim_end) {
               if (end_trim == 0)
                  pos = special;
               else
                  pos = pos | 0x80000000;
            } else {
               assert(end_trim < size && end_trim >= -1);
               if (!fastpath) assert(end_trim < size-1); // special always matches last one
               assert(end_trim < size && end_trim+1 >= 0);
               if (!fastpath) assert(end_trim+1 < size);

               if (mi.trim_start)
                  trim = start_trim*256 + (end_trim+1);
               else
                  trim = end_trim+1;

               assert(pos < 65536); //