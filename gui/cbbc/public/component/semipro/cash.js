class Cash extends React.Component {
  constructor(props) {
    super(props)
    this.state = {}
  }
  
  componentDidMount() {

  }
  
  render() {
    return(

<div className="row">
<div className="col-12 col-sm-12">
<table className="table table-striped table-cash">
<thead>
  <tr><th colSpan="2">現金</th></tr>
</thead>
<tbody>
  <tr>
  <td width="50">現金</td>
  <td width="50">借貸額</td>
  </tr>
  <tr>
  <td>-</td>
  <td>-</td>
  </tr>
</tbody>
</table>
</div>
</div>

    )
  }
}
