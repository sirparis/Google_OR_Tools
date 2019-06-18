"""ΜΑΘΗΜΑ ΕΠΙΧΕΙΡΗΣΙΑΚΗ ΕΡΕΥΝΑ, ΣΕΤ ΑΣΚΗΣΕΩΝ 1, 2.ΓΡΑΦΙΚΉ ΕΠΙΛΥΣΗ ΠΡΟΒΛΗΜΑΤΩΝ ΓΠ, ΑΣΚΗΣΗ 1 """
"""Linear optimization example"""

from __future__ import print_function
from ortools.linear_solver import pywraplp

def main():
  # Instantiate a Glop solver, naming it LinearExample.
  solver = pywraplp.Solver('LinearExample',
                           pywraplp.Solver.GLOP_LINEAR_PROGRAMMING)

# Create the two variables and let them take on any value.
  x = solver.NumVar(-solver.infinity(), solver.infinity(), 'x')
  y = solver.NumVar(-solver.infinity(), solver.infinity(), 'y')

# Constraint 1: 30x + 20y <= 300.
  constraint1 = solver.Constraint(-solver.infinity(), 300)
  constraint1.SetCoefficient(x, 30)
  constraint1.SetCoefficient(y, 20)

# Constraint 2: 5x + 10y <= 110.
  constraint2 = solver.Constraint(-solver.infinity(), 110)
  constraint2.SetCoefficient(x, 5)
  constraint2.SetCoefficient(y, 10)



# Objective function: 6x+8y.
  objective = solver.Objective()
  objective.SetCoefficient(x, 6)
  objective.SetCoefficient(y, 8)
  objective.SetMaximization()

# Solve the system.
  solver.Solve()
  opt_solution = 6 * x.solution_value() + 8 * y.solution_value()
  print('Number of variables =', solver.NumVariables())
  print('Number of constraints =', solver.NumConstraints())
# The value of each variable in the solution.
  print('Solution:')
  print('x = ', x.solution_value())
  print('y = ', y.solution_value())

# The objective value of the solution.
  print('Optimal objective value =', opt_solution)

  print(('%s: reduced cost = %f' % (x, x.reduced_cost())))
  print(('%s: reduced cost = %f' % (y, y.reduced_cost())))
  print(('%s: Dual Value = %f' % (y, constraint1.dual_value())))
  print(('%s: Dual Value = %f' % (y, constraint2.dual_value())))

if __name__ == '__main__':
  main()