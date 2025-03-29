# from here:
#
# https://github.com/lefticus/cppbestpractices/blob/master/02-Use_the_Tools_Available.md

function(set_project_warnings project_name)
  option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" TRUE)

  set(CLANG_WARNINGS
      -Werror      # We treat every warning as an error.
      -Weverything # Warn us about everything

      # disable useless warnings
      -Wno-c++98-compat               # no c++98 support
      -Wno-c++98-compat-pedantic      # no c++98 support
      -Wno-return-std-move-in-c++11   # no c++11 support
      -Wno-c++17-extensions           # no support below c++17
      -Wno-c++20-compat               # no support above c++20
      -Wno-pragmas                    # silence clang warnings that are not gcc warnings
      -Wno-unknown-warning-option     # silence gcc warnings that are not clang warnings
      -Wno-unused-macros              # some might be for debugging

      -Wno-missing-prototypes # C++ always requires prototypes

      -Wno-shadow-field-in-constructor # we dont start with m_variables!
  )

  if(WARNINGS_AS_ERRORS)
    set(CLANG_WARNINGS ${CLANG_WARNINGS} -Werror)
  endif()

  set(GCC_WARNINGS
      ${CLANG_WARNINGS}
      -Wmisleading-indentation # warn if indentation implies blocks where blocks do not exist
      -Wduplicated-cond # warn if if / else chain has duplicated conditions
      -Wduplicated-branches # warn if if / else branches have duplicated code
      -Wlogical-op # warn about logical operations being used where bitwise were probably wanted
      -Wuseless-cast # warn if you perform a cast to the same type
  )

  if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    set(PROJECT_WARNINGS ${CLANG_WARNINGS})
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(PROJECT_WARNINGS ${GCC_WARNINGS})
  else()
    message(AUTHOR_WARNING "No compiler warnings set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
  endif()

  target_compile_options(${project_name} INTERFACE ${PROJECT_WARNINGS})

endfunction()
