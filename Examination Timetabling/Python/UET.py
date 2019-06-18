#The following sections present a Python solution to the student scheduling problem.

from __future__ import print_function
from ortools.sat.python import cp_model



class studentsPartialSolutionPrinter(cp_model.CpSolverSolutionCallback):
    """Print intermediate solutions."""

    def __init__(self, shifts, num_students, num_days, num_shifts, sols):
        cp_model.CpSolverSolutionCallback.__init__(self)
        self._shifts = shifts
        self._num_students = num_students
        self._num_days = num_days
        self._num_shifts = num_shifts
        self._solutions = set(sols)
        self._solution_count = 0

    def on_solution_callback(self):
        self._solution_count += 1
        if self._solution_count in self._solutions:
            print('Solution %i' % self._solution_count)
            for d in range(self._num_days):
                print('Day %i' % d)
                for n in range(self._num_students):
                    is_working = False
                    for s in range(self._num_shifts):
                        if self.Value(self._shifts[(n, d, s)]):
                            is_working = True
                            print('  student %i works shift %i' % (n, s))
                    if not is_working:
                        print('  student {} does not work'.format(n))
            print()

    def solution_count(self):
        return self._solution_count




def main():
    # Data.
    num_students = 4
    num_shifts = 3
    num_days = 3
    all_students = range(num_students)
    all_shifts = range(num_shifts)
    all_days = range(num_days)
    # Creates the model.
    model = cp_model.CpModel()

    # Creates shift variables.
    # shifts[(n, d, s)]: student 'n' works shift 's' on day 'd'.
    shifts = {}
    for n in all_students:
        for d in all_days:
            for s in all_shifts:
                shifts[(n, d, s)] = model.NewBoolVar('shift_n%id%is%i' % (n, d,
                                                                          s))

    # Each shift is assigned to exactly one student in the schedule period.
    for d in all_days:
        for s in all_shifts:
            model.Add(sum(shifts[(n, d, s)] for n in all_students) == 1)

    # Each student works at most one shift per day.
    for n in all_students:
        for d in all_days:
            model.Add(sum(shifts[(n, d, s)] for s in all_shifts) <= 1)

    # min_shifts_per_student is the largest integer such that every student
    # can be assigned at least that many shifts. If the number of students doesn't
    # divide the total number of shifts over the schedule period,
    # some students have to work one more shift, for a total of
    # min_shifts_per_student + 1.
    min_shifts_per_student = (num_shifts * num_days) // num_students
    max_shifts_per_student = min_shifts_per_student + 1
    for n in all_students:
        num_shifts_worked = sum(
            shifts[(n, d, s)] for d in all_days for s in all_shifts)
        model.Add(min_shifts_per_student <= num_shifts_worked)
        model.Add(num_shifts_worked <= max_shifts_per_student)

    # Creates the solver and solve.
    solver = cp_model.CpSolver()
    solver.parameters.linearization_level = 0
    # Display the first five solutions.
    a_few_solutions = range(5)
    solution_printer = studentsPartialSolutionPrinter(
        shifts, num_students, num_days, num_shifts, a_few_solutions)
    solver.SearchForAllSolutions(model, solution_printer)

    # Statistics.
    print()
    print('Statistics')
    print('  - conflicts       : %i' % solver.NumConflicts())
    print('  - branches        : %i' % solver.NumBranches())
    print('  - wall time       : %f s' % solver.WallTime())
    print('  - solutions found : %i' % solution_printer.solution_count())


if __name__ == '__main__':
    main()